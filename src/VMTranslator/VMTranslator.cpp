#include "VMTranslator/VMTranslator.hpp"
#include "VMTranslator/VMCommandParser.hpp"
#include <iostream>
#include <stdexcept>
#include <memory>

using std::string;

VMTranslator::VMTranslator(const string& fileName, const string& inputDir, const string& outputDir, const bool debugMode)
    : debugMode_(debugMode)
{
    string vmFilePath = inputDir + fileName + ".vm";
    try {
        fileParser_ = std::make_unique<Parser>(vmFilePath);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("VM Translator failed to open/load input: " + string(e.what()));
    }
    string asmFilePath = outputDir + "/" + fileName + ".asm";
    codeWriter_ = std::make_unique<VMCodeWriter>(asmFilePath);
    codeWriter_->setFileName(fileName);
    currentFile_ = fileName;

    debugPrint("VM Translator initialized for file: " + fileName);
}


void VMTranslator::translate() {
    translateFile(); 
    closeWriter();
}

void VMTranslator::translateFile() {
    int codeLineNo = 0;
    while (fileParser_->hasMoreLines()) {
        string rawLine = fileParser_->advance();
        codeLineNo++;
        VMCommandParser currentCommand(rawLine);
        
        VMSpecifications::CommandType commandType = currentCommand.type();
        codeWriter_->writeAsComment(rawLine);
        if (commandType == VMSpecifications::CommandType::C_INVALID) {
            continue; 
        }
        
        if (commandType == VMSpecifications::CommandType::C_ARITHMETIC) {
            codeWriter_->writeArithmetic(currentCommand.arg1());
            
        } else if (commandType == VMSpecifications::CommandType::C_PUSH || 
                   commandType == VMSpecifications::CommandType::C_POP) 
        {
            const string& segment = currentCommand.arg1();
            int index = currentCommand.arg2();
            
            if (commandType == VMSpecifications::CommandType::C_PUSH) {
                codeWriter_->writePush(segment, index);
            } else {
                codeWriter_->writePop(segment, index);
            }
        } 
        // ... Logic for C_LABEL, C_GOTO, C_CALL, etc. will go here ...
        
        debugPrint("Line " + std::to_string(codeLineNo) + " translated: " + currentCommand.getCleanCommand());
    }
}

void VMTranslator::closeWriter() {
    if (codeWriter_) {
        codeWriter_->close();
    }
    fileParser_.reset();
}

void VMTranslator::debugPrint(const std::string& s) const {
    if (debugMode_) {
        std::cout << "[DEBUG] " << s << std::endl;
    }
}