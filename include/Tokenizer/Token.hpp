// Token.hpp
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include "TokenTypes.hpp"

class Token {
public:
    TokenType type;
    std::string lexeme;
    int line = 0; // 1-indexed line number
    int col = 0;  // 1-indexed column number

    Token(
        TokenType type = TokenType::ERROR,
        std::string lexeme = "",
        int line = 0,
        int col = 0
    );

    std::string toString() const;
    
    bool is(TokenType t) const { return type == t; }
    bool is(const std::string& lex) const { return lexeme == lex; }
    bool is(TokenType t, const std::string& lex) const { return type == t && lexeme == lex; }
};

#endif // TOKEN_HPP