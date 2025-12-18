#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <memory>
#include "parser.hpp"
#include "HackAssembler/SymbolTable.hpp"
#include "HackAssembler/AssemblyCommandParser.hpp" 
#include "HackAssembler/CodeTable.hpp"
#include "HackAssembler/ListingFileWriter.hpp"

namespace fs = std::filesystem;

class HackAssembler {
public:
    // Constructor: Initializes components and opens output files
    HackAssembler(const std::string& fileName, const std::string& inputDir, const std::string& outputDir, const bool debugMode = false, const bool generateListing = false);

    // Main assembly workflow methods
    void assemble();
    void firstPass();
    void secondPass();
    void listingsPass();
    void closeWriters();

private:
    SymbolTable symbolTable_;
    Parser parser_; // Your memory-based file buffer
    
    // Output File Streams
    std::ofstream hackWriter_;
    std::unique_ptr<ListingFileWriter> listWriter_;
    
    int codeLineNo_ = 0;
    const int listingSpacing_ = 10;
    bool debugMode_ = false;

    // Command processing helper
    void hackWriteAddress(const std::string& symbol, std::string& rawLine);

    // General utility functions
    static bool isNumeric(const std::string& str);
    static std::string getBinaryRepresentation(int n);
    std::string centerSpaces(const std::string& s);
    void debugPrint(const std::string& s);
};