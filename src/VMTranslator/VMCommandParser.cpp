#include "VMTranslator/VMCommandParser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>


VMCommandParser::VMCommandParser(const std::string& rawLine)
    : rawLine_(rawLine) 
{
    std::string tempCleanLine = stripLine(rawLine);
    
    if (!tempCleanLine.empty()) {
        parseAndIdentifyCommand(tempCleanLine);
        cleanCommand_ = tempCleanLine;
    }
}

const std::string& VMCommandParser::arg1() const {
if (commandType_ == VMSpecifications::CommandType::C_RETURN) {
        throw std::runtime_error("C_RETURN command has no arg1.");
    }
    if (commandType_ != VMSpecifications::CommandType::C_ARITHMETIC) {
        ensureCorrectArgumentCount(2); 
        return parts_[1];
    } 
    
    ensureCorrectArgumentCount(1);
    return parts_[0];
}

int VMCommandParser::arg2() const {
    if (commandType_ != VMSpecifications::CommandType::C_PUSH &&
        commandType_ != VMSpecifications::CommandType::C_POP &&
        commandType_ != VMSpecifications::CommandType::C_FUNCTION &&
        commandType_ != VMSpecifications::CommandType::C_CALL) 
    {
        throw std::runtime_error("Current command type does not require arg2.");
    }
    ensureCorrectArgumentCount(3);
    
    try {
        return std::stoi(parts_[2]);
    } catch (const std::exception& e) {
        throw std::runtime_error("Argument 2 is not a valid integer: " + parts_[2]);
    }
}


std::string VMCommandParser::stripLine(const std::string& line) {
    std::string stripped = line;

    size_t commentPos = stripped.find("//");
    if (commentPos != std::string::npos) {
        stripped.erase(commentPos);
    }

    stripped.erase(0, stripped.find_first_not_of(" \t\n\r"));
    stripped.erase(stripped.find_last_not_of(" \t\n\r") + 1);

    return stripped;
}

void VMCommandParser::parseAndIdentifyCommand(const std::string& cleanLine) {
    if (cleanLine.empty()) {
        commandType_ = VMSpecifications::CommandType::C_INVALID;
        return;
    }

    // 1. Split the clean line into tokens (words)
    std::stringstream ss(cleanLine);
    std::string token;
    while (ss >> token) {
        parts_.push_back(token);
    }
    
    if (parts_.empty()) {
        commandType_ = VMSpecifications::CommandType::C_INVALID;
        return;
    }

    const std::string& commandWord = parts_[0];

    if (VMSpecifications::ArithmeticUnaryOperators.count(commandWord) ||
        VMSpecifications::ArithmeticBinaryOperators.count(commandWord) ||
        VMSpecifications::ArithmeticCompareJumps.count(commandWord)) {
        
        commandType_ = VMSpecifications::CommandType::C_ARITHMETIC;
        return;
    }

    if (commandWord == "push") {
        commandType_ = VMSpecifications::CommandType::C_PUSH;
    } else if (commandWord == "pop") {
        commandType_ = VMSpecifications::CommandType::C_POP;
    } else if (commandWord == "label") {
        commandType_ = VMSpecifications::CommandType::C_LABEL;
    } else if (commandWord == "goto") {
        commandType_ = VMSpecifications::CommandType::C_GOTO;
    } else if (commandWord == "if-goto") {
        commandType_ = VMSpecifications::CommandType::C_IF_GOTO;
    } else if (commandWord == "call") {
        commandType_ = VMSpecifications::CommandType::C_CALL;
    } else if (commandWord == "function") {
        commandType_ = VMSpecifications::CommandType::C_FUNCTION;
    } else if (commandWord == "return") {
        commandType_ = VMSpecifications::CommandType::C_RETURN;
    } else {
        commandType_ = VMSpecifications::CommandType::C_INVALID;
    }
}

void VMCommandParser::ensureCorrectArgumentCount(int expected) const {
    if (parts_.size() < expected) {
        std::string msg = "Command requires " + std::to_string(expected) + 
                          " tokens but found only " + std::to_string(parts_.size()) +
                          ". Line: " + rawLine_;
        throw std::runtime_error(msg);
    }
}