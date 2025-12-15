#ifndef VM_COMMAND_PARSER_HPP
#define VM_COMMAND_PARSER_HPP

#include "VMTranslator/VMSpecififcations.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>

class VMCommandParser {
public:
    VMCommandParser(const std::string& rawLine);
    VMSpecifications::CommandType type() const { 
        return commandType_; 
    }
    const std::string& arg1() const;
    int arg2() const;
    bool isValidCommand() const {
        return commandType_ != VMSpecifications::CommandType::C_INVALID;
    }
    const std::string& getCleanCommand() const {
        return cleanCommand_;
    }

private:
    std::string rawLine_;
    std::string cleanCommand_;
    VMSpecifications::CommandType commandType_ = VMSpecifications::CommandType::C_INVALID;
    std::vector<std::string> parts_;
    std::string stripLine(const std::string& line);
    void parseAndIdentifyCommand(const std::string& cleanLine);
    void ensureCorrectArgumentCount(int expected) const;
};

#endif // VM_COMMAND_PARSER_HPP