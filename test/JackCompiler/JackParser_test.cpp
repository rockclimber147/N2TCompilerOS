#include <catch2/catch_test_macros.hpp>
#include <string>

#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"

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

    // Setup dependencies
    JackSpec spec;
    StreamTokenizer t(classDec, spec);
    TokenValidator val(t);
    JackParser p(val);

    // Act
    ClassIR classIR = p.parseClass();

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

    JackSpec spec;
    StreamTokenizer t(classDec, spec);
    TokenValidator val(t);
    JackParser p(val);

    ClassIR classIR = p.parseClass();

    REQUIRE(classIR.classVariables.size() == 3);
    CHECK(classIR.classVariables[0].name == "x");
    CHECK(classIR.classVariables[1].name == "y");
    CHECK(classIR.classVariables[2].name == "z");
    for(const auto& v : classIR.classVariables) {
        CHECK(v.type == "int");
        CHECK(v.kind == VarKind::STATIC);
    }
}