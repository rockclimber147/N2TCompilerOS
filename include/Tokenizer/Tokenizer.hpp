#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include "Tokenizer/Token.hpp"
#include "Tokenizer/TokenizerSpec.hpp"


class StreamTokenizer {
private:
    const TokenizerSpec& spec_;
    std::string content_;
    
    size_t start_ = 0;
    size_t end_ = 0;
    int line_ = 1;
    int column_ = 1;
    size_t lineStart_ = 0;

    Token currentToken_ = Token(TokenType::ERROR, "");
    std::unique_ptr<Token> nextToken_ = nullptr;

    void resetState();
    char next();
    char curr() const;
    char peek(int offset) const;
    Token scanNextToken();
    void scanIdentifierKeyword(Token& token);
    void scanNumericLiteral(Token& token);
    void scanStringLiteral(Token& token);
    void skipWhitespaceAndComments();
public:
    StreamTokenizer(const TokenizerSpec& spec);
    Token advance();
    Token peek();
    std::string getCurrentLineContent();
    void load(std::string content);
    void loadFromFile(const std::string& filePath);
};

#endif