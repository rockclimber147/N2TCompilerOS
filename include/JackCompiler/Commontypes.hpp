#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <string>

enum class VarKind {
    STATIC,      // Maps to 'static' segment
    FIELD,       // Maps to 'this' segment (object instance variables)
    ARG,         // Maps to 'argument' segment
    VAR          // Maps to 'local' segment
};

enum class SubroutineType {
    CONSTRUCTOR, // Allocates memory, sets 'this' to the new address
    FUNCTION,    // Static context (no 'this' pointer)
    METHOD       // Instance context ('argument 0' is the 'this' pointer)
};

inline std::string varKindToString(VarKind kind) {
    switch (kind) {
        case VarKind::STATIC: return "static";
        case VarKind::FIELD:  return "field";
        case VarKind::ARG:    return "argument";
        case VarKind::VAR:    return "local";
        default:              return "unknown";
    }
}

inline std::string subroutineTypeToString(SubroutineType type) {
    switch (type) {
        case SubroutineType::CONSTRUCTOR: return "constructor";
        case SubroutineType::FUNCTION:    return "function";
        case SubroutineType::METHOD:      return "method";
        default:                          return "unknown";
    }
}

#endif