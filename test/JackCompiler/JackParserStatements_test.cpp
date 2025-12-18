#include <catch2/catch_test_macros.hpp>
#include <string>

#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/IR/StatementIR.hpp"
#include "JackCompiler/IR/ExpressionIR.hpp"

inline ClassIR parseToClassIR(const std::string& content) {
    JackSpec spec;
    StreamTokenizer t(spec);
    t.load(content);
    TokenValidator val(t);
    JackParser p(val);
    
    return p.parseClass();
}


TEST_CASE("JackParser parses Return and Do statements", "[JackParser][Statement]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        do test();"
        "        return;"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    REQUIRE(classIR.subroutines.size() == 1);
    const auto& statements = classIR.subroutines[0].bodyStatements;

    REQUIRE(statements.size() == 2);
    CHECK(dynamic_cast<DoStatementIR*>(statements[0].get()) != nullptr);
    CHECK(dynamic_cast<ReturnStatementIR*>(statements[1].get()) != nullptr);
}

TEST_CASE("JackParser parses Let statements (simple and array)", "[JackParser][Statement]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        let x = true;"
        "        let arr[i] = true;"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    const auto& statements = classIR.subroutines[0].bodyStatements;

    REQUIRE(statements.size() == 2);

    auto* letSimple = dynamic_cast<LetStatementIR*>(statements[0].get());
    REQUIRE(letSimple != nullptr);
    CHECK(letSimple->varName == "x");
    CHECK(letSimple->indexExpression == nullptr);

    auto* letArray = dynamic_cast<LetStatementIR*>(statements[1].get());
    REQUIRE(letArray != nullptr);
    CHECK(letArray->varName == "arr");
    // Once parseExpression is implemented, indexExpression will not be nullptr here
}

TEST_CASE("JackParser parses While loops", "[JackParser][Statement]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        while (true) {"
        "            let x = true;"
        "        }"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    const auto& statements = classIR.subroutines[0].bodyStatements;

    REQUIRE(statements.size() == 1);
    auto* whileStmt = dynamic_cast<WhileStatementIR*>(statements[0].get());
    REQUIRE(whileStmt != nullptr);
    
    // Verify the body of the while loop contains the nested statement
    CHECK(whileStmt->body.size() == 1);
    CHECK(dynamic_cast<LetStatementIR*>(whileStmt->body[0].get()) != nullptr);
}

TEST_CASE("JackParser parses If/Else blocks", "[JackParser][Statement]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        if (isTrue) {"
        "            do test();"
        "        } else {"
        "            do test();"
        "            return;"
        "        }"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    const auto& statements = classIR.subroutines[0].bodyStatements;

    REQUIRE(statements.size() == 1);
    auto* ifStmt = dynamic_cast<IfStatementIR*>(statements[0].get());
    REQUIRE(ifStmt != nullptr);

    SECTION("If block contents") {
        CHECK(ifStmt->ifBlock.size() == 1);
        CHECK(dynamic_cast<DoStatementIR*>(ifStmt->ifBlock[0].get()) != nullptr);
    }

    SECTION("Else block contents") {
        CHECK(ifStmt->elseBlock.size() == 2);
        CHECK(dynamic_cast<DoStatementIR*>(ifStmt->elseBlock[0].get()) != nullptr);
        CHECK(dynamic_cast<ReturnStatementIR*>(ifStmt->elseBlock[1].get()) != nullptr);
    }
}

TEST_CASE("JackParser handles deeply nested statements", "[JackParser][Statement]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        while (a) {"
        "            if (b) {"
        "                while (c) { do test(); }"
        "            }"
        "        }"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    const auto& statements = classIR.subroutines[0].bodyStatements;

    // Outer While
    auto* while1 = dynamic_cast<WhileStatementIR*>(statements[0].get());
    REQUIRE(while1->body.size() == 1);

    // Inner If
    auto* if1 = dynamic_cast<IfStatementIR*>(while1->body[0].get());
    REQUIRE(if1->ifBlock.size() == 1);

    // Deepest While
    auto* while2 = dynamic_cast<WhileStatementIR*>(if1->ifBlock[0].get());
    REQUIRE(while2->body.size() == 1);
    CHECK(dynamic_cast<DoStatementIR*>(while2->body[0].get()) != nullptr);
}