// JackSpec.cpp

#include "JackCompiler/JackSpec.hpp"
#include <string>
#include <unordered_set>
#include <utility>

// --- I. PUBLIC CONSTANT STRINGS Initialization ---

// Keywords
const std::string JackSpec::CLASS         = "class";
const std::string JackSpec::CONSTRUCTOR   = "constructor";
const std::string JackSpec::FUNCTION      = "function";
const std::string JackSpec::METHOD        = "method";
const std::string JackSpec::FIELD         = "field";
const std::string JackSpec::STATIC        = "static";
const std::string JackSpec::VAR           = "var";
const std::string JackSpec::INT           = "int";
const std::string JackSpec::CHAR          = "char";
const std::string JackSpec::BOOLEAN       = "boolean";
const std::string JackSpec::VOID          = "void";
const std::string JackSpec::TRUE          = "true";
const std::string JackSpec::FALSE         = "false";
const std::string JackSpec::NULL_KEYWORD  = "null"; // Avoid conflict with C++ NULL
const std::string JackSpec::THIS          = "this";
const std::string JackSpec::LET           = "let";
const std::string JackSpec::DO            = "do";
const std::string JackSpec::IF            = "if";
const std::string JackSpec::ELSE          = "else";
const std::string JackSpec::WHILE         = "while";
const std::string JackSpec::RETURN        = "return";

// Symbols
const std::string JackSpec::L_CURLY_BRACKET = "{";
const std::string JackSpec::R_CURLY_BRACKET = "}";
const std::string JackSpec::L_PAREN         = "(";
const std::string JackSpec::R_PAREN         = ")";
const std::string JackSpec::L_SQUARE_BRACKET= "[";
const std::string JackSpec::R_SQUARE_BRACKET= "]";
const std::string JackSpec::DOT             = ".";
const std::string JackSpec::COMMA           = ",";
const std::string JackSpec::SEMICOLON       = ";";
const std::string JackSpec::PLUS            = "+";
const std::string JackSpec::MINUS           = "-";
const std::string JackSpec::MULTIPLY        = "*";
const std::string JackSpec::DIVIDE          = "/";
const std::string JackSpec::AMPERSAND       = "&";
const std::string JackSpec::PIPE            = "|";
const std::string JackSpec::LESS_THAN       = "<";
const std::string JackSpec::GREATER_THAN    = ">";
const std::string JackSpec::EQ              = "=";
const std::string JackSpec::TILDE           = "~"; // Unary Not


const std::unordered_set<std::string> JackSpec::BINARY_OPS = {
    PLUS, MINUS, MULTIPLY, DIVIDE, 
    AMPERSAND, PIPE, 
    LESS_THAN, GREATER_THAN, EQ
};

const std::unordered_set<std::string> JackSpec::KEYWORD_CONSTANTS = {
    TRUE, FALSE, NULL_KEYWORD, THIS
};

const std::unordered_set<std::string> JackSpec::JACK_KEYWORDS_SET = {
    JackSpec::CLASS, JackSpec::CONSTRUCTOR, JackSpec::FUNCTION, JackSpec::METHOD, 
    JackSpec::FIELD, JackSpec::STATIC, JackSpec::VAR, JackSpec::INT, 
    JackSpec::CHAR, JackSpec::BOOLEAN, JackSpec::VOID, JackSpec::TRUE, 
    JackSpec::FALSE, JackSpec::NULL_KEYWORD, JackSpec::THIS, JackSpec::LET, 
    JackSpec::DO, JackSpec::IF, JackSpec::ELSE, JackSpec::WHILE, JackSpec::RETURN
};

const std::unordered_set<std::string> JackSpec::JACK_SYMBOLS_SET = {
    JackSpec::L_CURLY_BRACKET, JackSpec::R_CURLY_BRACKET, JackSpec::L_PAREN, JackSpec::R_PAREN,
    JackSpec::L_SQUARE_BRACKET, JackSpec::R_SQUARE_BRACKET, JackSpec::DOT, JackSpec::COMMA, 
    JackSpec::SEMICOLON, JackSpec::PLUS, JackSpec::MINUS, JackSpec::MULTIPLY, 
    JackSpec::DIVIDE, JackSpec::AMPERSAND, JackSpec::PIPE, JackSpec::LESS_THAN, 
    JackSpec::GREATER_THAN, JackSpec::EQ, JackSpec::TILDE
};