#ifndef TOKEN_SPEC_HPP
#define TOKEN_SPEC_HPP

#include <string>
#include <unordered_set>

class TokenizerSpec {
public:
    const std::unordered_set<std::string> symbols; 
    const std::unordered_set<std::string> keywords;

    TokenizerSpec(
        std::unordered_set<std::string> symbolsList, 
        std::unordered_set<std::string> keywordsList
    ) : symbols(std::move(symbolsList)), keywords(std::move(keywordsList)) {}

    bool isSymbol(const std::string& lexeme) const {
        return symbols.count(lexeme) > 0;
    }

    bool isKeyword(const std::string& lexeme) const {
        return keywords.count(lexeme) > 0;
    }
};

#endif