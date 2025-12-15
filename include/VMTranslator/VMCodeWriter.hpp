#ifndef VMCODEWRITER_HPP
#define VMCODEWRITER_HPP

#include "VMTranslator/VMCommandParser.hpp"
#include "VMTranslator/VMSpecifications.hpp"
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>

class VMCodeWriter {
private:
    std::ofstream codeWriter_; // Mirrors ListingFileWriter's writer_
    
    int jumpTarget_ = 0;
    int returnTarget_ = 0;
    std::string currentFileName_ = ""; 
    
    // --- Private Helper Methods ---
    void write(const std::string& asmCode);
    void writeLine(const std::string& asmLine);
    void writePointerLoad(const std::string& segment, int index);
    void writeSPIncrement();
    void writeSPDecrement();
    
    void writeCallImpl(const std::string& functionName, int nArgs);
    void writeCalculateSegmentAddress(const std::string& segment, int index);
    
public:
    VMCodeWriter(const std::string& outputFilePath);

    ~VMCodeWriter() = default; 

    void setFileName(const std::string& vmFileName) { currentFileName_ = vmFileName; }
    
    void writeInit();
    void writeAsComment(const std::string& command);
    void writePush(const std::string& segment, int index);
    void writePop(const std::string& segment, int index);
    void writeArithmetic(const std::string& command);
    void writeLabel(const std::string& label);
    void writeGoTo(const std::string& label);
    void writeIf(const std::string& label);
    void writeFunction(const std::string& functionName, int nVars);
    void writeCall(const std::string& functionName, int nArgs);
    void writeReturn();
    
    void close();
};

#endif // VMCODEWRITER_HPP