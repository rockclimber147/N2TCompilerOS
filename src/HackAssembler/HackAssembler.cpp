#include "HackAssembler/HackAssembler.hpp"
#include "HackAssembler/ListingFileWriter.hpp"
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
    string hackFilePath = outputDir + "/" + fileName + ".hack";
    string listingFilePath = outputDir + "/" + fileName + ".listing.txt";

    hackWriter_.open(hackFilePath);
    listWriter_ = std::make_unique<ListingFileWriter>(listingFilePath);

    if (!hackWriter_.is_open()) {
        throw std::runtime_error("Could not open output hack file at " + hackFilePath);
    }
}

void HackAssembler::closeWriters() {
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
    listWriter_->writeHeader(); 
    
    while (parser_.hasMoreLines()) {
        if (!parser_.hasMoreLines()) int a = 5;
        string rawLine = parser_.advance();
        AssemblyCommandParser currentCommand(rawLine);
        auto command = currentCommand.type();

        if (command == AssemblyCommandParser::A_INSTRUCTION) {
            hackWriteAddress(currentCommand.symbol(), rawLine);
        } else if (command == AssemblyCommandParser::C_INSTRUCTION) {
            string d = currentCommand.dest(); 
            string c = currentCommand.comp();
            string j = currentCommand.jump();
            string codeBinary = "111" + CodeTable::comp(c) + CodeTable::dest(d) + CodeTable::jump(j) + "\n";

            listWriter_->writeCommand(codeLineNo_, rawLine);
            hackWriter_ << codeBinary;
        } else if (command == AssemblyCommandParser::L_INSTRUCTION) {
            string label = currentCommand.symbol();
            int romAddress = symbolTable_.getAddress(label); 
            
            listWriter_->writeLabel(romAddress, label, rawLine);
        } else {
            listWriter_->writeGeneric(rawLine);
        }
    }
}

void HackAssembler::hackWriteAddress(const string& symbol, std::string& rawLine) {
    string binaryRep;

    if (isNumeric(symbol)) {
        int constant = std::stoi(symbol);
        binaryRep = getBinaryRepresentation(constant);
        
        // CALL THE METHOD ON THE NEW WRITER OBJECT
        listWriter_->writeConstant(codeLineNo_, symbol, rawLine);
    } else {
        if (!symbolTable_.hasSymbol(symbol)) {
            symbolTable_.addVariable(symbol);
        }
        int varAddress = symbolTable_.getAddress(symbol);
        binaryRep = getBinaryRepresentation(varAddress);
        
        // CALL THE METHOD ON THE NEW WRITER OBJECT
        listWriter_->writeVarAddress(codeLineNo_, varAddress, rawLine);
    }
    hackWriter_ << binaryRep << "\n";
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