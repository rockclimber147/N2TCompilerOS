#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

class CodeTable {
public:
    CodeTable() = delete; 

    static std::string comp(const std::string& c);
    static std::string dest(const std::string& d);
    static std::string jump(const std::string& j);
    
private:
    static const std::unordered_map<std::string, std::string>& getDTable();
    static const std::unordered_map<std::string, std::string>& getCTable();
    static const std::unordered_map<std::string, std::string>& getJTable();
    
    static std::string lookup(const std::unordered_map<std::string, std::string>& table, const std::string& key);
};