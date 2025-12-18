#include "JackCompiler/VMWriter.hpp"

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

std::string VMWriter::segToString(Segment seg) {
    switch (seg) {
        case Segment::CONST:   return "constant";
        case Segment::ARG:     return "argument";
        case Segment::LOCAL:   return "local";
        case Segment::STATIC:  return "static";
        case Segment::THIS:    return "this";
        case Segment::THAT:    return "that";
        case Segment::POINTER: return "pointer";
        case Segment::TEMP:    return "temp";
        default:               return "unknown";
    }
}

void VMWriter::writePush(Segment seg, int index) {
    outFile << padding << "push " << segToString(seg) << " " << index << "\n";
}

void VMWriter::writePop(Segment seg, int index) {
    outFile << padding << "pop " << segToString(seg) << " " << index << "\n";
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