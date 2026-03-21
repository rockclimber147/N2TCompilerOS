#ifndef VM_PARSER_HPP
#define VM_PARSER_HPP

#include <string>
#include <vector>

class VMParser {
public:
    static std::vector<std::string> parse(const std::vector<std::string>& rawLines);

private:
    static std::string cleanLine(const std::string& line);
};

#endif