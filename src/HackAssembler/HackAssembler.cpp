#include "HackAssembler/HackAssembler.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <bitset>

using std::string;
using std::ofstream;

HackAssembler::HackAssembler(const string& fileName, const string& inputDir, const string& outputDir, const bool debugMode)
    : parser_(inputDir + fileName + ".asm") 
{
    debugMode_ = debugMode;
    string hackFilePath = outputDir + fileName + "/" + fileName + ".hack";
    string listingFilePath = outputDir + fileName + "/" + fileName + "_listing.txt";

    hackWriter_.open(hackFilePath);
    listWriter_.open(listingFilePath);

    if (!hackWriter_.is_open() || !listWriter_.is_open()) {
        throw std::runtime_error("Could not open output files.");
    }
}

void HackAssembler::closeWriters() {
    listWriter_.close();
    hackWriter_.close();
}

void HackAssembler::assemble() {
    firstPass();
    parser_.resetIndex();
    secondPass();
}

void HackAssembler::firstPass() {
    codeLineNo_ = 0;
    
    while (parser_.hasMoreLines()) {
        string rawLine = parser_.advance();
        AssemblyCommandParser currentCommand(rawLine);
        auto command = currentCommand.type();
        debugPrint("Line: " + std::to_string(codeLineNo_) + " | Type: " + std::to_string(command));

        if (command == AssemblyCommandParser::C_INSTRUCTION ||
            command == AssemblyCommandParser::A_INSTRUCTION) 
        {
            codeLineNo_++;
        } else if (command == AssemblyCommandParser::L_INSTRUCTION) {
            string label = currentCommand.symbol();
            symbolTable_.addJumpLabel(label, codeLineNo_); // Use current codeLineNo
        }
    }
}

void HackAssembler::secondPass() {
    codeLineNo_ = 0;
    listingWriteHeader();
    while (parser_.hasMoreLines()) {
        string rawLine = parser_.advance();
        AssemblyCommandParser currentCommand(rawLine);
        auto command = currentCommand.type();

        if (command == AssemblyCommandParser::A_INSTRUCTION) {
            hackWriteAddress(currentCommand.symbol());
        } else if (command == AssemblyCommandParser::C_INSTRUCTION) {
            string d = currentCommand.dest(); 
            string c = currentCommand.comp();
            string j = currentCommand.jump();
            string codeBinary = "111" + CodeTable::comp(c) + CodeTable::dest(d) + CodeTable::jump(j) + "\n";

            listingWriteCommand();
            hackWriter_ << codeBinary;
        } else if (command == AssemblyCommandParser::L_INSTRUCTION) {
            listingWriteLabel(currentCommand.symbol());
        } else {
            listingWriteGeneric(rawLine);
        }
    }
}

void HackAssembler::hackWriteAddress(const string& symbol) {
    string binaryRep;

    if (isNumeric(symbol)) {
        int constant = std::stoi(symbol);
        binaryRep = getBinaryRepresentation(constant);
        listingWriteConstant(symbol);
    } else {
        if (!symbolTable_.hasSymbol(symbol)) {
            symbolTable_.addVariable(symbol);
        }
        int varAddress = symbolTable_.getAddress(symbol);
        binaryRep = getBinaryRepresentation(varAddress);
        listingWriteVarAddress(varAddress);
    }
    hackWriter_ << binaryRep << "\n";
}

void HackAssembler::listingWriteHeader() {
    listWriter_ << centerSpaces("ROM") << "|"
                << centerSpaces("Address") << "| Source\n";
}

void HackAssembler::listingWriteGeneric(const std::string& line) {
    listWriter_ << std::string(listingSpacing_, ' ') << "|"
                << std::string(listingSpacing_, ' ') << "| "
                << line << "\n";
}

void HackAssembler::listingWriteVarAddress(int varAddress) {
    std::string lineToWrite = centerSpaces(std::to_string(codeLineNo_)) + "|" +
                              centerSpaces("RAM[" + std::to_string(varAddress) + "]") + "| "
                              + parser_.getCurrentLine();
    listWriter_ << lineToWrite << "\n";
    codeLineNo_++;
}

void HackAssembler::listingWriteConstant(const std::string& constant) {
    std::string lineToWrite = centerSpaces(std::to_string(codeLineNo_)) + "|" +
                              centerSpaces(constant) + "| "
                              + parser_.getCurrentLine();
    listWriter_ << lineToWrite << "\n";
    codeLineNo_++;
}

void HackAssembler::listingWriteCommand() {
    std::string lineToWrite = centerSpaces(std::to_string(codeLineNo_)) + "|" +
                              centerSpaces("") + "| " 
                              + parser_.getCurrentLine();
    listWriter_ << lineToWrite << "\n";
    codeLineNo_++;
}

void HackAssembler::listingWriteLabel(const std::string& label) {
    int romAddress = symbolTable_.getAddress(label); 

    std::string lineToWrite = centerSpaces(std::to_string(codeLineNo_)) + "|" +
                              centerSpaces("ROM[" + std::to_string(romAddress) + "]") +
                              "| " + parser_.getCurrentLine();
    listWriter_ << lineToWrite << "\n";
}

void HackAssembler::debugPrint(const std::string& s) {
    if (debugMode_) {
        std::cout << s << std::endl;
    }
}

bool HackAssembler::isNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    return std::all_of(str.begin(), str.end(), 
                       [](unsigned char c){ return std::isdigit(c); });
}

std::string HackAssembler::centerSpaces(const std::string& s) {
    int repeat = listingSpacing_ - static_cast<int>(s.length());
    if (repeat <= 0) {
        return s;
    }
    
    int left, right;
    if (repeat % 2 == 1) {
        left = repeat / 2;
        right = repeat / 2 + 1;
    } else {
        left = repeat / 2;
        right = repeat / 2;
    }
    return std::string(left, ' ') + s + std::string(right, ' ');
}

std::string HackAssembler::getBinaryRepresentation(int n) {
    if (n < 0 || n > 32767) { 
        throw std::out_of_range("Address value out of the 15-bit range (0-32767).");
    }
    std::string binaryRep = std::bitset<16>(n).to_string();

    return binaryRep;
}