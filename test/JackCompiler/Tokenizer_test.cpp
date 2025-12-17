#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <string>
#include <sstream>
#include <vector>

#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/Token.hpp"
#include "Tokenizer/TokenTypes.hpp"
#include "Tokenizer/TokenizerSpec.hpp"
#include "JackCompiler/JackSpec.hpp"

using namespace Catch::Matchers;

void REQUIRE_TOKEN(StreamTokenizer& t, TokenType expectedType, const std::string& expectedLexeme) {
    Token token = t.advance();
    REQUIRE(token.type == expectedType);
    REQUIRE(token.lexeme == expectedLexeme);
}

TEST_CASE("JackTokenizer: 1. Symbols and EOF", "[JackCompiler][Symbols]") {
    JackSpec const jack_spec{};
    const std::vector<std::string> SYMBOLS_LIST = {
        JackSpec::L_CURLY_BRACKET, JackSpec::R_CURLY_BRACKET, JackSpec::L_PAREN, JackSpec::R_PAREN,
        JackSpec::L_SQUARE_BRACKET, JackSpec::R_SQUARE_BRACKET, JackSpec::DOT, JackSpec::COMMA, 
        JackSpec::SEMICOLON, JackSpec::PLUS, JackSpec::MINUS, JackSpec::MULTIPLY, 
        JackSpec::DIVIDE, JackSpec::AMPERSAND, JackSpec::PIPE, JackSpec::LESS_THAN, 
        JackSpec::GREATER_THAN, JackSpec::EQ, JackSpec::TILDE
    };
    
    std::string all_symbols;
    for (const auto& symbol : SYMBOLS_LIST) {
        all_symbols += symbol;
    }
    
    StreamTokenizer t(all_symbols, jack_spec);

    for (const auto& symbol : SYMBOLS_LIST) {
        INFO("Testing symbol: " << symbol);
        REQUIRE_TOKEN(t, TokenType::SYMBOL, symbol);
    }
    
    REQUIRE(t.advance().type == TokenType::EOF_TYPE);
}

TEST_CASE("JackTokenizer: 2. Keywords", "[JackCompiler][Keywords]") {
    JackSpec const jack_spec{};
    const std::vector<std::string> ALL_JACK_KEYWORDS = {
        JackSpec::CLASS, JackSpec::CONSTRUCTOR, JackSpec::FUNCTION, JackSpec::METHOD, 
        JackSpec::FIELD, JackSpec::STATIC, JackSpec::VAR, JackSpec::INT, 
        JackSpec::CHAR, JackSpec::BOOLEAN, JackSpec::VOID, JackSpec::TRUE, 
        JackSpec::FALSE, JackSpec::NULL_KEYWORD, JackSpec::THIS, JackSpec::LET, 
        JackSpec::DO, JackSpec::IF, JackSpec::ELSE, JackSpec::WHILE, JackSpec::RETURN
    };

    std::string keyword_src; 
    for (const auto& keyword : ALL_JACK_KEYWORDS) {
        keyword_src += " " + keyword;
    }
    keyword_src += " ";

    StreamTokenizer t(keyword_src, jack_spec);

    for (const auto& keyword : ALL_JACK_KEYWORDS) {
        INFO("Testing keyword: " << keyword);
        REQUIRE_TOKEN(t, TokenType::KEYWORD, keyword);
    }
    REQUIRE(t.advance().type == TokenType::EOF_TYPE);
}

TEST_CASE("JackTokenizer: 3. Identifiers", "[JackCompiler][Identifiers]") {
    std::string id_src = " myClass _a123 myFunction ";
    JackSpec const jack_spec{};
    StreamTokenizer t(id_src, jack_spec);

    REQUIRE_TOKEN(t, TokenType::IDENTIFIER, "myClass");
    REQUIRE_TOKEN(t, TokenType::IDENTIFIER, "_a123");
    REQUIRE_TOKEN(t, TokenType::IDENTIFIER, "myFunction");
    REQUIRE(t.advance().type == TokenType::EOF_TYPE);
}

TEST_CASE("JackTokenizer: 4. Literals", "[JackCompiler][Literals]") {
    std::string literal_src = R"( 123 "hello world" 32767 )";
    JackSpec const jack_spec{};
    StreamTokenizer t(literal_src, jack_spec);

    REQUIRE_TOKEN(t, TokenType::INTEGER_LITERAL, "123");
    
    REQUIRE_TOKEN(t, TokenType::STRING_LITERAL, "hello world");
    
    REQUIRE_TOKEN(t, TokenType::INTEGER_LITERAL, "32767");
    REQUIRE(t.advance().type == TokenType::EOF_TYPE);
}

TEST_CASE("JackTokenizer: 5. Whitespace and Comments", "[JackCompiler][Skip]") {
    std::string skip_src = 
        "\t\r// Comment one\n"
        "/* Multiline\n"
        "   Comment\n"
        "*/"
        + JackSpec::CLASS + " // inline comment\n"
        + JackSpec::INT
        + "/* adjacent block */"
        + JackSpec::WHILE;
    JackSpec const jack_spec{};
    StreamTokenizer t(skip_src, jack_spec);

    REQUIRE_TOKEN(t, TokenType::KEYWORD, JackSpec::CLASS);
    REQUIRE_TOKEN(t, TokenType::KEYWORD, JackSpec::INT);
    REQUIRE_TOKEN(t, TokenType::KEYWORD, JackSpec::WHILE);
    REQUIRE(t.advance().type == TokenType::EOF_TYPE);
}

TEST_CASE("JackTokenizer: 6. Error Cases", "[JackCompiler][Errors]") {
    std::string error_str = R"("unterminated string)";
    JackSpec const jack_spec{};
    StreamTokenizer t_str(error_str, jack_spec);
    REQUIRE_THROWS_AS(t_str.advance(), std::runtime_error);

    std::string error_newline = "\"string\nbreak\"";
    StreamTokenizer t_nl(error_newline, jack_spec);
    REQUIRE_THROWS_AS(t_nl.advance(), std::runtime_error);

    std::string error_comment = "/* unterminated";
    StreamTokenizer t_cmt(error_comment, jack_spec);
    REQUIRE_THROWS_AS(t_cmt.advance(), std::runtime_error);
}