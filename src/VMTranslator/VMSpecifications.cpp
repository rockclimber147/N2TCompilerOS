#include "VMTranslator/VMSpecifications.hpp"

const std::unordered_map<std::string, int> VMSpecifications::SegmentPointerAddresses = {
    {"local", 1},
    {"argument", 2},
    {"this", 3},
    {"that", 4}
};

const std::unordered_map<std::string, std::string> VMSpecifications::ArithmeticUnaryOperators = {
    {"neg", "-"},
    {"not", "!"}
};

const std::unordered_map<std::string, std::string> VMSpecifications::ArithmeticBinaryOperators = {
    {"add", "+"},
    {"sub", "-"},
    {"and", "&"},
    {"or", "|"}
};

const std::unordered_map<std::string, std::string> VMSpecifications::ArithmeticCompareJumps = {
    {"eq", "JEQ"},
    {"gt", "JGT"},
    {"lt", "JLT"}
};

const std::set<std::string> VMSpecifications::validCommands = {
    "push",
    "pop",
    "add",
    "sub",
    "neg",
    "and",
    "or",
    "not",
    "eq",
    "gt",
    "lt",
    "label",
    "goto",
    "if-goto",
    "call",
    "function",
    "return"
};