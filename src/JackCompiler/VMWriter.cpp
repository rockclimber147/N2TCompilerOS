#include <string>

#include "JackCompiler/VMWriter.hpp"
#include "JackCompiler/Commontypes.hpp"

VMWriter::VMWriter(const std::string& fileName) {
    outFile.open(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << fileName << std::endl;
    }
}

VMWriter::~VMWriter() {
    if (outFile.is_open()) {
        close();
    }
}

void VMWriter::close() {
    outFile << "// end" << std::endl;
    outFile.close();
}

std::string VMWriter::kindToSegment(VarKind kind) {
    switch (kind) {
        case VarKind::STATIC: return "static";
        case VarKind::FIELD:  return "this";
        case VarKind::ARG:    return "argument";
        case VarKind::VAR:    return "local";
        default: return "unknown";
    }
}

void VMWriter::writePush(VarKind kind, int index) {
    outFile << padding << "push " << kindToSegment(kind) << " " << index << "\n";
}

void VMWriter::writePop(VarKind kind, int index) {
    outFile << padding << "pop " << kindToSegment(kind) << " " << index << "\n";
}

void VMWriter::writeArithmetic(const std::string& command) {
    outFile << padding << command << "\n";
}

void VMWriter::writeLabel(const std::string& label) {
    outFile << "label " << label << "\n";
}

void VMWriter::writeGoTo(const std::string& label) {
    outFile << padding << "goto " << label << "\n";
}

void VMWriter::writeIf(const std::string& label) {
    outFile << padding << "if-goto " << label << "\n";
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
    outFile << padding << "call " << name << " " << nArgs << "\n";
}

void VMWriter::writeFunction(const std::string& name, int nVars) {
    outFile << "\nfunction " << name << " " << nVars << "\n";
}

void VMWriter::writeReturn() {
    outFile << padding << "return\n";
}

void VMWriter::writeComment(const std::string& comment) {
    outFile << "// " << comment << "\n";
}