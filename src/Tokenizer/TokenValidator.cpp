#include "Tokenizer/TokenValidator.hpp" 
#include "JackCompiler/JackSpec.hpp"
#include <sstream>
#include <stdexcept>


TokenValidator::TokenValidator(StreamTokenizer& tokenizer) 
: tokenizer_ (tokenizer) 
{ }

Token TokenValidator::peekNext() {
    return tokenizer_.peek();
}

Token TokenValidator::expectType(TokenType type) {
    Token token = tokenizer_.advance();
    if (token.type == type) return token;

    throwTokenError(token, "Expected type " + tokenTypeToString(type) + 
                           ", but found " + tokenTypeToString(token.type) + 
                           " with lexeme '" + token.lexeme + "'.");
}

Token TokenValidator::expectSpecific(const std::string& lexeme) { // Changed argument to const&
    Token token = tokenizer_.advance();
    if (token.lexeme == lexeme) return token;

    throwTokenError(token, "Expected specific lexeme '" + lexeme + 
                           "', but found '" + token.lexeme + "' (" + tokenTypeToString(token.type) + ").");
}

Token TokenValidator::expectOneOfLexemes(std::initializer_list<std::string> expectedLexemes) {
    Token token = tokenizer_.advance();
    auto it = std::find(expectedLexemes.begin(), expectedLexemes.end(), token.lexeme);

    if (it == expectedLexemes.end()) {
        throwTokenError(
            token,
            "Expected one of the lexemes " + formatExpected(expectedLexemes) + 
            ", but found '" + token.lexeme + "'."
        );
    }
    return token;
}

Token TokenValidator::expectOneOfTypes(std::initializer_list<TokenType> expectedTypes) {
    Token token = tokenizer_.advance();
    bool typeMatch = false;

    for (TokenType expectedType : expectedTypes) {
        if (token.type == expectedType) {
            typeMatch = true;
            break;
        }
    }

    if (!typeMatch) {
        throwTokenError(
            token,
            "Found token '" + token.lexeme + "' (" + tokenTypeToString(token.type) + 
            "), but expected one of the specified types."
        );
    }
    return token;
}

std::string TokenValidator::formatExpected(std::initializer_list<std::string> expected) const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& item : expected) {
        if (!first) {
            oss << ", ";
        }
        oss << "'" << item << "'";
        first = false;
    }
    oss << "}";
    return oss.str();
}

void TokenValidator::throwTokenError(Token token, std::string message) {
    message = "\n[L" + std::to_string(token.line) + ":C" + std::to_string(token.col) + "] Parse Error: " + message +
              "\n\n" + tokenizer_.getCurrentLineContent() +
              "\n" + std::string(token.col - 1, ' ') + std::string(token.lexeme.length(), '^') +
              "\n";

    throw std::runtime_error(message);
}