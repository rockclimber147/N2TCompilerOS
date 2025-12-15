#ifndef VM_SPECIFICATIONS_HPP
#define VM_SPECIFICATIONS_HPP

#include <string>
#include <unordered_map>
#include <set>

class VMSpecifications {
public:

    enum class CommandType {
        C_ARITHMETIC,
        C_PUSH,
        C_POP,
        C_LABEL,
        C_GOTO,
        C_IF_GOTO,
        C_CALL,
        C_FUNCTION,
        C_RETURN,
        C_INVALID
    };

    static const std::unordered_map<std::string, int> SegmentPointerAddresses;
    static const std::unordered_map<std::string, std::string> ArithmeticUnaryOperators;
    static const std::unordered_map<std::string, std::string> ArithmeticBinaryOperators;
    static const std::unordered_map<std::string, std::string> ArithmeticCompareJumps;

    static const std::set<std::string> validCommands;

    VMSpecifications() = delete;

    static const std::string TEMP;
    static const std::string POINTER;
    static const std::string THIS;
    static const std::string THAT;
    static const std::string STATIC;
    static const std::string CONSTANT;
};

#endif