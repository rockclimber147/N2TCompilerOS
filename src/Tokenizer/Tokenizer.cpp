#include "Tokenizer/Tokenizer.hpp"
#include <iostream>
#include <utility>
#include <stdexcept>
#include <cctype>
#include <string>

using namespace std;

StreamTokenizer::StreamTokenizer(std::string content, const TokenizerSpec& spec)
    : spec_(spec),
      content_(std::move(content)),
      start_(0),
      end_(0),
      line_(1),
      column_(1),
      lineStart_(0)
{
}


Token StreamTokenizer::advance() {
    if (nextToken_ == nullptr) {
        currentToken_ = scanNextToken();
    } else {
        currentToken_ = std::move(*nextToken_);
        nextToken_ = nullptr;
    }
    return currentToken_;
}

Token StreamTokenizer::peek() {
    if (nextToken_ == nullptr) {
        nextToken_ = std::make_unique<Token>(scanNextToken());
    }
    return *nextToken_;
}

char StreamTokenizer::peek(int offset) const {
    if (end_ + offset >= content_.length()) {
        return '\0';
    }
    return content_[end_ + offset];
}


std::string StreamTokenizer::getCurrentLineContent() {
    size_t endOfLineIndex = lineStart_;
    while (
        endOfLineIndex < content_.length() &&
        content_[endOfLineIndex] != '\n' &&
        content_[endOfLineIndex] != '\r'
    ) {
        endOfLineIndex++;
    }
    return content_.substr(lineStart_, endOfLineIndex - lineStart_);
}

char StreamTokenizer::next() { 
    if (end_ >= content_.length()) {
        return '\0';
    }
    char consumedChar = content_[end_++];
    if (consumedChar == '\n') {
        line_++;
        column_ = 1;
        lineStart_ = end_;
    } else {
        column_++;
    }
    return consumedChar;
}

char StreamTokenizer::curr() const {
    if (end_ >= content_.length()) {
        return '\0';
    }
    return content_[end_];
}

Token StreamTokenizer::scanNextToken() {
    start_ = end_;
    // scan whitespace comments here
    start_ = end_;

    Token token(TokenType::ERROR, "", line_, column_);

    if (end_ >= content_.length()) {
        token.type = TokenType::EOF_TYPE;
        return token;
    }

    const char c = next();
    std::string char_str(1, c);

    if (spec_.symbols.count(char_str) > 0) {
        token.type = TokenType::SYMBOL;
        token.lexeme = char_str;
    } else if (c == '_' || std::isalpha(static_cast<unsigned char>(c))) {
        scanIdentifierKeyword(token);
    } else if (std::isdigit(static_cast<unsigned char>(c))) {
        scanNumericLiteral(token);
    } else if (c == '"') {
        scanStringLiteral(token);
    }

    return token;
}

void StreamTokenizer::scanIdentifierKeyword(Token& token) {
    while (std::isdigit(static_cast<unsigned char>(curr())) || 
    std::isalpha(static_cast<unsigned char>(curr())) || 
    curr() == '_') {
        next();
    }
    token.lexeme = content_.substr(start_, end_ - start_);
    if (spec_.isKeyword(token.lexeme)) {
        token.type = TokenType::KEYWORD;
    } else {
        token.type = TokenType::IDENTIFIER;
    }
}

void StreamTokenizer::scanNumericLiteral(Token& token) {
    while (std::isdigit(static_cast<unsigned char>(curr()))) {
        next();
    }
    token.lexeme = content_.substr(start_, end_ - start_);
    token.type = TokenType::INTEGER_LITERAL;
}   

void StreamTokenizer::scanStringLiteral(Token& token) {
    token.type = TokenType::STRING_LITERAL;
    const int startLine = token.line;
    const int startCol = token.col;

    while (curr() != '"' && curr() != '\0') {
        char current = curr();

        if (current == '\n' || current == '\r') {
            throw std::runtime_error("Lexical Error: Unterminated string literal (newline found) starting at line " 
                                     + std::to_string(startLine) + " col " + std::to_string(startCol));
        }
        next(); 
    }

    if (curr() == '\0') {
        throw std::runtime_error("Lexical Error: Unterminated string literal (EOF reached) starting at line " 
                                 + std::to_string(startLine) + " col " + std::to_string(startCol));
    }
    
    next();
    
    std::string fullLexeme = content_.substr(start_, end_ - start_);
    
    token.lexeme = fullLexeme.substr(1, fullLexeme.length() - 2); 
}

void StreamTokenizer::skipWhitespaceAndComments() {
    bool keepSkipping = true;
    while (keepSkipping) {
        char current = curr();
        if (std::isspace(static_cast<unsigned char>(current))) {
            next(); 
            
        } else if (current == '/' && peek(1) == '/') {
            next(); 
            next(); 
            
            while (curr() != '\n' && curr() != '\0') {
                next();
            }
            next();
            
        } else if (current == '/' && peek(1) == '*') {
            next();
            next();
            
            while (true) {
                if (curr() == '\0') {
                    throw std::runtime_error("Lexical Error: Unterminated multi-line comment starting at line " 
                                             + std::to_string(line_) + " col " + std::to_string(column_));
                }
                if (curr() == '*' && peek(1) == '/') {
                    next();
                    next();
                    break; 
                }
                next(); 
            }
            
        } else {
            keepSkipping = false;
        }
    }
}