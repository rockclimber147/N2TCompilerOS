#ifndef VMTRANSLATOR_HPP
#define VMTRANSLATOR_HPP

#include "VMTranslator/VMCommandParser.hpp"
#include "VMTranslator/VMSpecifications.hpp"
#include "VMTranslator/VMCodeWriter.hpp"
#include "parser.hpp" // <-- Include the generic file parser
#include <fstream>
#include <string>
#include <memory> 

class VMTranslator {
private:
    std::string currentFile_ = "";
    std::unique_ptr<Parser> fileParser_;

    std::unique_ptr<VMCodeWriter> codeWriter_;
    
    bool debugMode_ = false;

    void debugPrint(const std::string& s) const;
    void translateFile();
    
public:
    VMTranslator(const std::string& fileName, const std::string& inputDir, const std::string& outputDir, const bool debugMode);
    
    void translate();
    
    void closeWriter();
};

#endif