#ifndef VM_WRITER_HPP
#define VM_WRITER_HPP

#include <fstream>
#include <string>
#include <iostream>
#include "JackCompiler/Commontypes.hpp"

enum class Segment {
    CONST,
    ARG,
    LOCAL,
    STATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

class VMWriter {
public:
    explicit VMWriter(const std::string& fileName);
    ~VMWriter();

    void writePush(Segment seg, int index);
    void writePop(Segment seg, int index);
    void writeArithmetic(const std::string& command);
    
    void writeLabel(const std::string& label);
    void writeGoTo(const std::string& label);
    void writeIf(const std::string& label);
    
    void writeCall(const std::string& name, int nArgs);
    void writeFunction(const std::string& name, int nVars);
    void writeReturn();

    void writeComment(const std::string& comment);
    void close();

private:
    std::ofstream outFile;
    const std::string padding = "    ";
    std::string segToString(Segment seg);
    std::string kindToSegment(VarKind kind);
};

#endif