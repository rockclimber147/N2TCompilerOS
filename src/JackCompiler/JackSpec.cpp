// JackSpec.cpp

#include "JackCompiler/JackSpec.hpp"
#include <string>
#include <unordered_set>
#include <utility>

const std::unordered_set<std::string> JackSpec::JACK_KEYWORDS = {
    // Class/Function structure
    "class", "constructor", "function", "method", "field", "static", "var", 
    
    // Primitive types
    "int", "char", "boolean", "void", 
    
    // Constant keywords
    "true", "false", "null", "this", 
    
    // Statements
    "let", "do", "if", "else", "while", "return"
};

const std::unordered_set<std::string> JackSpec::JACK_SYMBOLS = {
    // Grouping / Structure
    "{", "}", "(", ")", "[", "]", 
    
    // Delimiters
    ".", ",", ";", 
    
    // Operators
    "+", "-", "*", "/", "&", "|", "<", ">", "=", 
    
    // Unary Operator
    "~" 
};