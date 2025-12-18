#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/SemanticAnalyzer.hpp"
#include "JackCompiler/SymbolTable.hpp"

inline ClassIR parseToClassIR(const std::string& content) {
    JackSpec spec;
    StreamTokenizer t(content, spec);
    TokenValidator val(t);
    JackParser p(val);
    return p.parseClass();
}

template<typename... Args>
std::vector<ClassIR> buildProject(Args&&... codeStrings) {
    std::vector<ClassIR> project;
    (project.push_back(std::move(parseToClassIR(codeStrings))), ...);
    return project;
}

// --- Tests ---

TEST_CASE("Semantic Analyzer: Valid cross-class method call", "[semantic]") {
    auto project = buildProject(
        R"(class Square {
            field int size;
            constructor Square new(int s) {
                let size = s;
                return this;
            }
            method void grow() {
                let size = size + 1;
                return;
            }
        })",
        R"(class Main {
            function void main() {
                var Square s;
                let s = Square.new(10);
                do s.grow();
                return;
            }
        })"
    );

    ProjectSymbolTable table;
    SemanticAnalyzer analyzer(table);

    REQUIRE_NOTHROW(analyzer.analyze(project));

    ClassEntry* squareEntry = table.getClass("Square");
    REQUIRE(squareEntry != nullptr);
    
    // Check field 'size'
    const VariableSymbol* sizeVar = squareEntry->lookup("size");
    REQUIRE(sizeVar != nullptr);
    CHECK(sizeVar->kind == VarKind::FIELD);
    CHECK(sizeVar->index == 0);

    auto it = squareEntry->subroutines.find("grow");
    REQUIRE(it != squareEntry->subroutines.end());
    SubroutineEntry& growSub = it->second;

    // Check implicit 'this' argument for method
    const VariableSymbol* thisArg = growSub.lookup("this", squareEntry);
    REQUIRE(thisArg != nullptr);
    CHECK(thisArg->kind == VarKind::ARG);
    CHECK(thisArg->index == 0);

    ClassEntry* mainEntry = table.getClass("Main");
    SubroutineEntry& mainSub = mainEntry->subroutines.at("main");

    const VariableSymbol* sVar = mainSub.lookup("s", mainEntry);
    REQUIRE(sVar != nullptr);
    CHECK(sVar->type == "Square");
    CHECK(sVar->kind == VarKind::VAR);
    CHECK(sVar->index == 0);
}

TEST_CASE("Semantic Analyzer: Verify Multiple Symbol Indices", "[semantic]") {
    auto project = buildProject(R"(
        class Game {
            static int count;      // static 0
            field int x, y;        // field 0, field 1
            static int score;      // static 1

            method void reset(int startX, int startY) { // arg 0 is 'this', 1 is startX, 2 is startY
                var int a, b, c;   // var 0, 1, 2
                return;
            }
        }
    )");

    ProjectSymbolTable table;
    SemanticAnalyzer analyzer(table);
    analyzer.analyze(project);

    ClassEntry* cls = table.getClass("Game");
    
    // Verify Class Statics vs Fields
    CHECK(cls->lookup("count")->index == 0);
    CHECK(cls->lookup("count")->kind == VarKind::STATIC);
    CHECK(cls->lookup("score")->index == 1);
    CHECK(cls->lookup("score")->kind == VarKind::STATIC);
    
    CHECK(cls->lookup("x")->index == 0);
    CHECK(cls->lookup("x")->kind == VarKind::FIELD);
    CHECK(cls->lookup("y")->index == 1);
    CHECK(cls->lookup("y")->kind == VarKind::FIELD);

    // Verify Subroutine Args and Vars
    SubroutineEntry& sub = cls->subroutines.at("reset");
    CHECK(sub.lookup("this", cls)->index == 0);
    CHECK(sub.lookup("startX", cls)->index == 1);
    CHECK(sub.lookup("startY", cls)->index == 2);
    
    CHECK(sub.lookup("a", cls)->index == 0);
    CHECK(sub.lookup("c", cls)->index == 2);
}

TEST_CASE("Semantic Analyzer: Catch field access in static function", "[semantic]") {
    auto project = buildProject(R"(
        class Player {
            field int score;
            function void reset() {
                let score = 0; 
                return;
            }
        }
    )");

    ProjectSymbolTable table;
    SemanticAnalyzer analyzer(table);

    REQUIRE_THROWS_AS(analyzer.analyze(project), std::runtime_error);
}

TEST_CASE("Semantic Analyzer: Undeclared variable", "[semantic]") {
    auto project = buildProject(R"(
        class Test {
            method void run() {
                let x = 5; 
                return;
            }
        }
    )");

    ProjectSymbolTable table;
    SemanticAnalyzer analyzer(table);

    REQUIRE_THROWS_AS(analyzer.analyze(project), std::runtime_error);
}

TEST_CASE("Semantic Analyzer: Illegal static call to method", "[semantic]") {
    auto project = buildProject(
        "class Graphics { method void draw() { return; } }",
        R"(class Main {
            function void main() {
                do Graphics.draw(); 
                return;
            }
        })"
    );
    
    ProjectSymbolTable table;
    SemanticAnalyzer analyzer(table);

    REQUIRE_THROWS_AS(analyzer.analyze(project), std::runtime_error);
}