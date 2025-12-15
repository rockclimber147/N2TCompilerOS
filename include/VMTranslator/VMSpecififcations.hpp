#ifndef VM_SPECIFICATIONS_HPP
#define VM_SPECIFICATIONS_HPP

#include <string>
#include <unordered_map>
#include <set>

class VMSpecifications {
public:
    static const std::unordered_map<std::string, int> SegmentPointerAddresses;
    static const std::unordered_map<std::string, std::string> ArithmeticUnaryOperators;
    static const std::unordered_map<std::string, std::string> ArithmeticBinaryOperators;
    static const std::unordered_map<std::string, std::string> ArithmeticCompareJumps;

    static const std::set<std::string> validCommands;
    
    VMSpecifications() = delete;
};

#endif