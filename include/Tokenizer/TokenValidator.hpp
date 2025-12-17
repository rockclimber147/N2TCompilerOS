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

    Token expectOneOfTypes(std::initializer_list<TokenType> expectedTypes);
    
    std::string formatExpected(std::initializer_list<std::string> expected) const;

    void throwTokenError(Token token, std::string message);
};