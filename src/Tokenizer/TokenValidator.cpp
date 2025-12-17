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

template <typename T>
std::string TokenValidator::formatCollection(const T& collection) const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& item : collection) {
        if (!first) oss << ", ";
        oss << "'" << item << "'";
        first = false;
    }
    oss << "}";
    return oss.str();
}


Token TokenValidator::expectOneOfLexemes(const std::unordered_set<std::string>& expected) {
    Token t = tokenizer_.advance();
    if (expected.find(t.lexeme) == expected.end()) 
        throwTokenError(t, "Expected one of " + formatCollection(expected) + ", found '" + t.lexeme + "'");
    return t;
}

Token TokenValidator::expectOneOfLexemes(std::initializer_list<std::string> expected) {
    Token t = tokenizer_.advance();
    if (std::find(expected.begin(), expected.end(), t.lexeme) == expected.end())
        throwTokenError(t, "Expected one of " + formatCollection(expected) + ", found '" + t.lexeme + "'");
    return t;
}


Token TokenValidator::expectOneOfTypes(const std::unordered_set<TokenType>& expected) {
    Token t = tokenizer_.advance();
    if (expected.find(t.type) == expected.end())
        throwTokenError(t, "Expected different type, found '" + t.lexeme + "' (" + tokenTypeToString(t.type) + ")");
    return t;
}

Token TokenValidator::expectOneOfTypes(std::initializer_list<TokenType> expected) {
    Token t = tokenizer_.advance();
    if (std::find(expected.begin(), expected.end(), t.type) == expected.end())
        throwTokenError(t, "Expected different type, found '" + t.lexeme + "' (" + tokenTypeToString(t.type) + ")");
    return t;
}


[[noreturn]] void TokenValidator::throwTokenError(Token token, std::string message) {
    message = "\n[L" + std::to_string(token.line) + ":C" + std::to_string(token.col) + "] Parse Error: " + message +
              "\n\n" + tokenizer_.getCurrentLineContent() +
              "\n" + std::string(token.col - 1, ' ') + std::string(token.lexeme.length(), '^') +
              "\n";

    throw std::runtime_error(message);
}