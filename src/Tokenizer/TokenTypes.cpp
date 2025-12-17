#include "Tokenizer/TokenTypes.hpp"
#include <stdexcept>
#include <string>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EOF_TYPE: return "EOF_TYPE";
        case TokenType::ERROR: return "ERROR";
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        default: return "UNKNOWN_TYPE";
    }
}