#ifndef TOKEN_TYPES_HPP
#define TOKEN_TYPES_HPP

#include <string>

enum class TokenType {
    EOF_TYPE,
    ERROR,
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INTEGER_LITERAL,
    STRING_LITERAL
};

std::string tokenTypeToString(TokenType type);

#endif