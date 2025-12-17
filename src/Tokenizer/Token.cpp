#include "Tokenizer/Token.hpp"
#include "Tokenizer/TokenTypes.hpp"

Token::Token(
    TokenType type,
    std::string lexeme,
    int line,
    int col
) : type(type), lexeme(std::move(lexeme)), line(line), col(col) 
{}

std::string Token::toString() const {
    return tokenTypeToString(type) + ": \"" + lexeme + "\" @ line " + 
           std::to_string(line) + " col " + std::to_string(col);
}