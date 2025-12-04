#include "HackAssembler/CodeTable.hpp"

using std::string;
using std::unordered_map;

// --- Private Static Map Initialization ---
const unordered_map<string, string>& CodeTable::getDTable() {
    static const unordered_map<string, string> dTable = {
        {"", "000"},
        {"M", "001"},
        {"D", "010"},
        {"MD", "011"},
        {"A", "100"},
        {"AM", "101"},
        {"AD", "110"},
        {"AMD", "111"}
    };
    return dTable;
}

const unordered_map<string, string>& CodeTable::getJTable() {
    static const unordered_map<string, string> jTable = {
        {"", "000"},
        {"JGT", "001"},
        {"JEQ", "010"},
        {"JGE", "011"},
        {"JLT", "100"},
        {"JNE", "101"},
        {"JLE", "110"},
        {"JMP", "111"}
    };
    return jTable;
}

const unordered_map<string, string>& CodeTable::getCTable() {
    static const unordered_map<string, string> cTable = {
{"0", "0101010"},
        {"1",   "0111111"},
        {"-1",  "0111010"},
        {"D",   "0001100"},
        {"A",   "0110000"},
        {"M",   "1110000"},
        {"!D",  "0001101"},
        {"!A",  "0110001"},
        {"!M",  "1110001"},
        {"-D",  "0001111"},
        {"-A",  "0110011"},
        {"-M",  "1110011"},
        {"D+1", "0011111"},
        {"A+1", "0110111"},
        {"M+1", "1110111"},
        {"D-1", "0001110"},
        {"A-1", "0110010"},
        {"M-1", "1110010"},
        {"D+A", "0000010"},
        {"A+D", "0000010"},
        {"D+M", "1000010"},
        {"M+D", "1000010"},
        {"D-A", "0010011"},
        {"D-M", "1010011"},
        {"A-D", "0000111"},
        {"M-D", "1000111"},
        {"D&A", "0000000"},
        {"A&D", "0000000"},
        {"D&M", "1000000"},
        {"M&D", "1000000"},
        {"D|A", "0010101"},
        {"A|D", "0010101"},
        {"D|M", "1010101"},
        {"M|D", "1010101"}
    };
    return cTable;
}

string CodeTable::lookup(const unordered_map<string, string>& table, const string& key) {
    auto it = table.find(key);
    if (it != table.end()) {
        return it->second;
    }
    throw std::out_of_range("Invalid assembly mnemonic found: " + key);
}

string CodeTable::comp(const string& c) {
    return lookup(getCTable(), c);
}

string CodeTable::dest(const string& d) {
    return lookup(getDTable(), d);
}

string CodeTable::jump(const string& j) {
    return lookup(getJTable(), j);
}