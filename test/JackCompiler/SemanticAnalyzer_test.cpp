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