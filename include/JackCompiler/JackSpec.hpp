// JackSpec.hpp
#ifndef JACK_SPEC_HPP
#define JACK_SPEC_HPP

#include "Tokenizer/TokenizerSpec.hpp"
#include <unordered_set>
#include <string>
#include <utility> 


class JackSpec : public TokenizerSpec {
public:
    // --- PUBLIC CONSTANT STRINGS (For external comparison) ---
    // These strings are used by the Parser and compiler logic (e.g., if (token.lexeme == JackSpec::CLASS))
    
    // Keywords (21 total)
    static const std::string CLASS;
    static const std::string CONSTRUCTOR;
    static const std::string FUNCTION;
    static const std::string METHOD;
    static const std::string FIELD;
    static const std::string STATIC;
    static const std::string VAR;
    static const std::string INT;
    static const std::string CHAR;
    static const std::string BOOLEAN;
    static const std::string VOID;
    static const std::string TRUE;
    static const std::string FALSE;
    static const std::string NULL_KEYWORD;
    static const std::string THIS;
    static const std::string LET;
    static const std::string DO;
    static const std::string IF;
    static const std::string ELSE;
    static const std::string WHILE;
    static const std::string RETURN;

    // Symbols (19 total)
    static const std::string L_CURLY_BRACKET; // "{"
    static const std::string R_CURLY_BRACKET; // "}"
    static const std::string L_PAREN;         // "("
    static const std::string R_PAREN;         // ")"
    static const std::string L_SQUARE_BRACKET; // "["
    static const std::string R_SQUARE_BRACKET; // "]"
    static const std::string DOT;             // "."
    static const std::string COMMA;           // ","
    static const std::string SEMICOLON;       // ";"
    static const std::string PLUS;            // "+"
    static const std::string MINUS;           // "-"
    static const std::string MULTIPLY;        // "*"
    static const std::string DIVIDE;          // "/"
    static const std::string AMPERSAND;       // "&"
    static const std::string PIPE;            // "|"
    static const std::string LESS_THAN;       // "<"
    static const std::string GREATER_THAN;    // ">"
    static const std::string EQ;              // "="
    static const std::string TILDE;           // "~" (Unary NOT)

    /**
     * @brief The constructor initializes the base TokenizerSpec 
     * with the private static sets.
     */
    JackSpec() 
        : TokenizerSpec(
            JACK_SYMBOLS_SET, 
            JACK_KEYWORDS_SET
        ) 
    {}

    static const std::unordered_set<std::string> BINARY_OPS;
    static const std::unordered_set<std::string> UNARY_OPS;
    static const std::unordered_set<std::string> KEYWORD_CONSTANTS;

private:
    static const std::unordered_set<std::string> JACK_KEYWORDS_SET;
    static const std::unordered_set<std::string> JACK_SYMBOLS_SET;
};

#endif