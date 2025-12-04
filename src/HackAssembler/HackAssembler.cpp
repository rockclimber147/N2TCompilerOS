#include "HackAssembler/HackAssembler.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

using std::string;
using std::ofstream;

HackAssembler::HackAssembler(const string& fileName, const string& inputDir, const string& outputDir)
    : parser_(inputDir + fileName + ".asm") 
{
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
    // Access SymbolTable address using the provided SymbolTable::getAddress method
    int romAddress = symbolTable_.getAddress(label); 

    std::string lineToWrite = centerSpaces(std::to_string(codeLineNo_)) + "|" +
                              centerSpaces("ROM[" + std::to_string(romAddress) + "]") +
                              "| " + parser_.getCurrentLine();
    listWriter_ << lineToWrite << "\n";
    // NOTE: Labels do NOT increment codeLineNo_ in Pass 2, only A/C commands do.
}

void HackAssembler::debugPrint(const std::string& s) {
    if (debugMode_) {
        std::cout << s << std::endl;
    }
}