#ifndef JACK_SPEC_HPP
#define JACK_SPEC_HPP

#include "Tokenizer/TokenizerSpec.hpp"
#include <unordered_set>
#include <string>
#include <utility> 

class JackSpec : public TokenizerSpec {
private:
    static const std::unordered_set<std::string> JACK_KEYWORDS;
    static const std::unordered_set<std::string> JACK_SYMBOLS;

public:
    JackSpec() 
        : TokenizerSpec(
            JACK_SYMBOLS, 
            JACK_KEYWORDS
        ) 
    {}
};

#endif