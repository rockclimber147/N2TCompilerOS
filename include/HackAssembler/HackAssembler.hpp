#pragma once

#include <string>
#include <fstream>
#include <memory>
#include "parser.hpp"                           // Your file buffer/iterator
#include "HackAssembler/SymbolTable.hpp"
#include "HackAssembler/AssemblyCommandParser.hpp" 
#include "HackAssembler/CodeTable.hpp"          // Static utility

class HackAssembler {
public:
    // Constructor: Initializes components and opens output files
    HackAssembler(const std::string& fileName, const std::string& inputDir, const std::string& outputDir);

    // Main assembly workflow methods
    void assemble();
    void firstPass();
    void secondPass();
    void closeWriters();

private:
    SymbolTable symbolTable_;
    Parser parser_; // Your memory-based file buffer
    
    // Output File Streams
    std::ofstream hackWriter_;
    std::ofstream listWriter_;
    
    int codeLineNo_ = 0;
    const int listingSpacing_ = 10;
    bool debugMode_ = false;

    // Command processing helper
    void hackWriteAddress(const std::string& symbol);

    // Utility functions (Implementations for listing, binary conversion, etc.)
   void listingWriteHeader();
    void listingWriteGeneric(const std::string& line);
    void listingWriteVarAddress(int varAddress);
    void listingWriteConstant(const std::string& constant);
    void listingWriteCommand();
    void listingWriteLabel(const std::string& label);

    // General utility functions
    static bool isNumeric(const std::string& str);
    static std::string getBinaryRepresentation(int n);
    std::string centerSpaces(const std::string& s);
    void debugPrint(const std::string& s);
};