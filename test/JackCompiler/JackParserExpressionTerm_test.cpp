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


#include <catch2/catch_test_macros.hpp>
#include <string>

// ... [Existing Includes and parseToClassIR helper] ...

TEST_CASE("JackParser handles basic literals as terms", "[JackParser][Expression]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        let x = 42;"
        "        let s = \"hello\";"
        "        let b = true;"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    const auto& body = classIR.subroutines[0].bodyStatements;

    // Test Integer
    auto* let1 = dynamic_cast<LetStatementIR*>(body[0].get());
    CHECK(dynamic_cast<IntegerLiteralIR*>(let1->assignedExpression.get()) != nullptr);

    // Test String
    auto* let2 = dynamic_cast<LetStatementIR*>(body[1].get());
    auto* str = dynamic_cast<StringLiteralIR*>(let2->assignedExpression.get());
    REQUIRE(str != nullptr);
    CHECK(str->value == "hello");

    // Test Keyword
    auto* let3 = dynamic_cast<LetStatementIR*>(body[2].get());
    auto* kw = dynamic_cast<KeywordLiteralIR*>(let3->assignedExpression.get());
    REQUIRE(kw != nullptr);
    CHECK(kw->keyword == "true");
}

TEST_CASE("JackParser handles binary operator associativity (Left-to-Right)", "[JackParser][Expression]") {
    // In Jack, 1 + 2 + 3 is parsed as (1 + 2) + 3
    std::string classDec = "class Test { function void main() { let x = 1 + 2 + 3; } }";
    ClassIR classIR = parseToClassIR(classDec);
    
    auto* let = dynamic_cast<LetStatementIR*>(classIR.subroutines[0].bodyStatements[0].get());
    auto* root = dynamic_cast<BinaryExpressionIR*>(let->assignedExpression.get());

    REQUIRE(root != nullptr);
    CHECK(root->op == "+");
    
    // The right side of the root should be the literal 3
    CHECK(dynamic_cast<IntegerLiteralIR*>(root->rightTerm.get()) != nullptr);
    
    // The left side of the root should be another BinaryExpression (1 + 2)
    auto* leftSubTree = dynamic_cast<BinaryExpressionIR*>(root->leftTerm.get());
    REQUIRE(leftSubTree != nullptr);
    CHECK(leftSubTree->op == "+");
}

TEST_CASE("JackParser handles complex nested expressions and calls", "[JackParser][Expression]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        let x = Math.sqrt((a + 1) + -y[i]);"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    auto* let = dynamic_cast<LetStatementIR*>(classIR.subroutines[0].bodyStatements[0].get());
    
    // Root should be a SubroutineCallIR
    auto* call = dynamic_cast<SubroutineCallIR*>(let->assignedExpression.get());
    REQUIRE(call != nullptr);
    CHECK(call->target == "Math");
    CHECK(call->methodName == "sqrt");
    REQUIRE(call->arguments.size() == 1);

    // Argument is (a + 1) * -y[i] -> Top level is Binary '*'
    auto* mult = dynamic_cast<BinaryExpressionIR*>(call->arguments[0].get());
    REQUIRE(mult != nullptr);
    CHECK(mult->op == "+");

    // Left of '*' is (a + 1). Note: Parentheses are stripped in IR, 
    // but the structure preserves the grouping.
    auto* group = dynamic_cast<BinaryExpressionIR*>(mult->leftTerm.get());
    REQUIRE(group != nullptr);
    CHECK(group->op == "+");

    // Right of '*' is unary minus '-' on an array access 'y[i]'
    auto* unary = dynamic_cast<UnaryTermIR*>(mult->rightTerm.get());
    REQUIRE(unary != nullptr);
    CHECK(unary->op == "-");

    auto* arrayVar = dynamic_cast<VariableTermIR*>(unary->term.get());
    REQUIRE(arrayVar != nullptr);
    CHECK(arrayVar->name == "y");
    CHECK(arrayVar->arrayIndex != nullptr); // The 'i'
}

TEST_CASE("JackParser handles empty and multi-argument expression lists", "[JackParser][Expression]") {
    std::string classDec = 
        "class Test {"
        "    function void main() {"
        "        do noArgs();"
        "        do manyArgs(1, x, 2+2);"
        "    }"
        "}";

    ClassIR classIR = parseToClassIR(classDec);
    const auto& body = classIR.subroutines[0].bodyStatements;

    auto* do1 = dynamic_cast<DoStatementIR*>(body[0].get());
    auto* call1 = dynamic_cast<SubroutineCallIR*>(do1->subroutineCall.get());
    CHECK(call1->arguments.empty());

    auto* do2 = dynamic_cast<DoStatementIR*>(body[1].get());
    auto* call2 = dynamic_cast<SubroutineCallIR*>(do2->subroutineCall.get());
    REQUIRE(call2->arguments.size() == 3);
    CHECK(dynamic_cast<IntegerLiteralIR*>(call2->arguments[0].get()) != nullptr);
    CHECK(dynamic_cast<VariableTermIR*>(call2->arguments[1].get()) != nullptr);
    CHECK(dynamic_cast<BinaryExpressionIR*>(call2->arguments[2].get()) != nullptr);
}