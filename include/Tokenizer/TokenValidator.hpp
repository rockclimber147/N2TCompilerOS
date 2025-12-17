#ifndef TOKEN_VALIDATOR_HPP
#define TOKEN_VALIDATOR_HPP

#include <string>
#include <initializer_list>
#include <algorithm>

#include "Tokenizer/TokenTypes.hpp"
#include "Tokenizer/Tokenizer.hpp"

class TokenValidator {
private:
    StreamTokenizer& tokenizer_;

public:    
    TokenValidator(StreamTokenizer& tokenizer);
    Token peekNext();
    Token expectType(TokenType type);
    Token expectSpecific(const std::string& lexeme);

    Token expectOneOfLexemes(std::initializer_list<std::string> expectedLexemes);
    Token expectOneOfLexemes(const std::unordered_set<std::string>& expectedLexemes);

    Token expectOneOfTypes(std::initializer_list<TokenType> expectedTypes);
    Token expectOneOfTypes(const std::unordered_set<TokenType>& expectedTypes);
    

    template <typename T>
    std::string formatCollection(const T& collection) const;

    [[noreturn]] void throwTokenError(Token token, std::string message);
};

#endif