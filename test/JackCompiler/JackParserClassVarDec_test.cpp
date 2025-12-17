#include <catch2/catch_test_macros.hpp>
#include <string>

#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"

inline ClassIR parseToClassIR(const std::string& content) {
    JackSpec spec;
    StreamTokenizer t(content, spec);
    TokenValidator val(t);
    JackParser p(val);
    
    return p.parseClass();
}



TEST_CASE("JackParser parses empty class", "[JackParser]") {
    std::string classDec = "class Test {}";
    ClassIR classIR = parseToClassIR(classDec);

    SECTION("Class metadata is correct") {
        CHECK(classIR.name == "Test");
        CHECK(classIR.classVariables.size() == 0);
        CHECK(classIR.subroutines.size() == 0);
    }
}

TEST_CASE("JackParser catches keyword issues", "[JackParser]") {
    std::string classDec = "clas Test {}";
    SECTION("Invalid 'class' keyword throws runtime_error") {
        CHECK_THROWS_AS(parseToClassIR(classDec), std::runtime_error);
    }

    SECTION("Misspelled 'field' in classVarDec") {
        std::string badField = "class T { feild boolean isOpen; }";
        CHECK_THROWS_AS(parseToClassIR(badField), std::runtime_error);
    }

    SECTION("Misspelled subroutine type") {
        std::string badFunc = "class T { funciton void main() {} }";
        CHECK_THROWS_AS(parseToClassIR(badFunc), std::runtime_error);
    }
}

TEST_CASE("JackParser parses minimal class", "[JackParser]") {
    std::string classDec = 
        "class Test {"
        "    static int count;"
        "    field boolean isOpen;"
        "    function void run() {"
        "    }"
        "    method char getKey() {"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);

    SECTION("Class metadata is correct") {
        CHECK(classIR.name == "Test");
        CHECK(classIR.classVariables.size() == 2);
        CHECK(classIR.subroutines.size() == 2);
    }

    SECTION("Class variables are parsed correctly") {
        // static int count;
        CHECK(classIR.classVariables[0].name == "count");
        CHECK(classIR.classVariables[0].type == "int");
        CHECK(classIR.classVariables[0].kind == VarKind::STATIC);

        // field boolean isOpen;
        CHECK(classIR.classVariables[1].name == "isOpen");
        CHECK(classIR.classVariables[1].type == "boolean");
        CHECK(classIR.classVariables[1].kind == VarKind::FIELD);
    }

    SECTION("Subroutines are parsed correctly") {
        // function void run()
        const auto& sub1 = classIR.subroutines[0];
        CHECK(sub1.name == "run");
        CHECK(sub1.returnType == "void");
        CHECK(sub1.type == SubroutineType::FUNCTION);
        CHECK(sub1.fullyQualifiedName == "Test.run");

        // method char getKey()
        const auto& sub2 = classIR.subroutines[1];
        CHECK(sub2.name == "getKey");
        CHECK(sub2.returnType == "char");
        CHECK(sub2.type == SubroutineType::METHOD);
        CHECK(sub2.fullyQualifiedName == "Test.getKey");
    }
}

TEST_CASE("JackParser handles multiple variables on one line", "[JackParser]") {
    std::string classDec = 
        "class MultiVar {"
        "    static int x, y, z;"
        "}";

    ClassIR classIR = parseToClassIR(classDec);

    REQUIRE(classIR.classVariables.size() == 3);
    CHECK(classIR.classVariables[0].name == "x");
    CHECK(classIR.classVariables[1].name == "y");
    CHECK(classIR.classVariables[2].name == "z");
    for(const auto& v : classIR.classVariables) {
        CHECK(v.type == "int");
        CHECK(v.kind == VarKind::STATIC);
    }
}

TEST_CASE("JackParser catches subroutine body errors", "[JackParser]") {
    SECTION("Missing comma in parameter list") {
        std::string badParams = "class T { method void m(int x int y) {} }";
        CHECK_THROWS_AS(parseToClassIR(badParams), std::runtime_error);
    }

    SECTION("Missing semicolon in var declaration") {
        std::string badVar = "class T { function void f() { var int x } }";
        CHECK_THROWS_AS(parseToClassIR(badVar), std::runtime_error);
    }
}

TEST_CASE("JackParser handles subroutine parameters", "[JackParser]") {
    std::string classDec = 
        "class ParamTest {"
        "    method void update(int x, boolean state, MyClass obj) {"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    REQUIRE(classIR.subroutines.size() == 1);
    const auto& sub = classIR.subroutines[0];

    SECTION("Parameter count and metadata") {
        REQUIRE(sub.parameters.size() == 3);
        
        CHECK(sub.parameters[0].name == "x");
        CHECK(sub.parameters[0].type == "int");
        CHECK(sub.parameters[0].kind == VarKind::ARG);

        CHECK(sub.parameters[1].name == "state");
        CHECK(sub.parameters[1].type == "boolean");
        CHECK(sub.parameters[1].kind == VarKind::ARG);

        CHECK(sub.parameters[2].name == "obj");
        CHECK(sub.parameters[2].type == "MyClass");
        CHECK(sub.parameters[2].kind == VarKind::ARG);
    }
}

TEST_CASE("JackParser handles complex subroutine bodies", "[JackParser]") {
    std::string classDec = 
        "class BodyTest {"
        "    function int compute() {"
        "        var int i, j;"
        "        var char key;"
        "        var Array arr;"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    REQUIRE(classIR.subroutines.size() == 1);
    const auto& sub = classIR.subroutines[0];

    SECTION("Local variables are correctly aggregated") {
        // We expect 4 total variables from 3 'var' lines
        REQUIRE(sub.locals.size() == 4);

        // First var line: var int i, j;
        CHECK(sub.locals[0].name == "i");
        CHECK(sub.locals[0].type == "int");
        CHECK(sub.locals[1].name == "j");
        CHECK(sub.locals[1].type == "int");

        // Second var line: var char key;
        CHECK(sub.locals[2].name == "key");
        CHECK(sub.locals[2].type == "char");

        // Third var line: var Array arr;
        CHECK(sub.locals[3].name == "arr");
        CHECK(sub.locals[3].type == "Array");

        // All should be VarKind::VAR
        for(const auto& v : sub.locals) {
            CHECK(v.kind == VarKind::VAR);
        }
    }
}

