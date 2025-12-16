#include "VMTranslator/VMCodeWriter.hpp"
#include "VMTranslator/VMSpecifications.hpp"
#include <filesystem> // For creating output directories

// --- CONSTRUCTOR & DESTRUCTION ---

VMCodeWriter::VMCodeWriter(const std::string& outputFilePath) 
    : jumpTarget_(0),
      returnTarget_(0)
{
    try {
        codeWriter_.open(outputFilePath, std::ios::out);
        
        if (!codeWriter_.is_open()) {
            throw std::runtime_error("Failed to open output file for writing. Check if directory exists: " + outputFilePath);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("File I/O setup failed: " + std::string(e.what()));
    }
    
    std::cout << "Successfully opened file for writing: " << outputFilePath << std::endl;
}

void VMCodeWriter::close() {
    if (codeWriter_.is_open()) {
        codeWriter_.close();
    }
}


void VMCodeWriter::write(const std::string& asmCode) {
    if (codeWriter_.is_open()) {
        codeWriter_ << asmCode;
    }
}

void VMCodeWriter::writeLine(const std::string& asmLine) {
    if (codeWriter_.is_open()) {
        codeWriter_ << asmLine << "\n";
    }
}
void VMCodeWriter::writeSPIncrement() {
    writeLine("@SP");
    writeLine("M=M+1");
}

void VMCodeWriter::writeSPDecrement() {
    writeLine("@SP");
    writeLine("M=M-1");
}

void VMCodeWriter::writePointerLoad(const std::string& segment, int index) {
    // TODO: Implement the logic to load the address of segment[index] into D.
}


// --- CORE WRITING METHODS ---

void VMCodeWriter::writeAsComment(const std::string& command) {
    writeLine("// " + command);
}

void VMCodeWriter::writeInit() {
    // 1. Set SP = 256
    writeLine("@256");
    writeLine("D=A");
    writeLine("@SP");
    writeLine("M=D");
}

void VMCodeWriter::writeLabel(const std::string& label) {
    writeLine("(" + label + ")");
}

void VMCodeWriter::writeGoTo(const std::string& label) {
    writeLine("@" + label);
    writeLine("0;JMP");
}

void VMCodeWriter::writeIf(const std::string& label) {
    writeLine("@0");
    writeLine("AM=M-1");
    writeLine("D=M");
    writeLine("@" + label);
    writeLine("D;JNE");
}

void VMCodeWriter::writePush(const std::string& segment, int index) {
    if (segment == VMSpecifications::CONSTANT) {
        writeLine("@" + std::to_string(index));
        writeLine("D=A"); 
    } else if (segment == VMSpecifications::TEMP) {
        writeLine("@" + std::to_string(5 + index));
        writeLine("D=M"); 
    } else if (segment == VMSpecifications::POINTER) {
        writeLine("@" + std::to_string(3 + index));
        writeLine("D=M");
    } else if (segment == VMSpecifications::STATIC) {
        writeLine("@" + currentFileName_ + "." + std::to_string(index)); 
        writeLine("D=M"); 
    } else if (VMSpecifications::SegmentPointerAddresses.count(segment)) {
        writeCalculateSegmentAddress(segment, index);
        
        writeLine("D=M"); 
    } else {
        std::cerr << "Invalid push segment: " << segment << std::endl;
        return;
    }
    writeLine("@SP");
    writeLine("A=M");
    writeLine("M=D");
    writeSPIncrement(); 
}

void VMCodeWriter::writePop(const std::string& segment, int index) {
    if (segment == VMSpecifications::TEMP) {
        writeLine("@" + std::to_string(5 + index));
        writeLine("D=A"); 
        writeLine("@R13");
        writeLine("M=D");
        
    } else if (segment == VMSpecifications::POINTER) {
        writeSPDecrement();
        writeLine("A=M");
        writeLine("D=M");

        writeLine("@" + std::to_string(3 + index));
        writeLine("M=D");
        return;

    } else if (segment == VMSpecifications::STATIC) {
        writeSPDecrement();
        writeLine("A=M"); 
        writeLine("D=M");

        writeLine("@" + currentFileName_ + "." + std::to_string(index));
        writeLine("M=D"); 
        return; 
        
    } else if (VMSpecifications::SegmentPointerAddresses.count(segment)) {
        writeCalculateSegmentAddress(segment, index); 
        writeLine("D=A");
        
        writeLine("@R13");
        writeLine("M=D");
        
    } else {
        std::cerr << "Invalid pop segment: " << segment << std::endl;
        return;
    }

    writeSPDecrement();
    writeLine("A=M"); 
    writeLine("D=M");
    
    writeLine("@R13");
    writeLine("A=M");
    writeLine("M=D"); 
}

void VMCodeWriter::writeCalculateSegmentAddress(const std::string& segment, int index) {
    writeLine("@" + std::to_string(index));
    writeLine("D=A");
    if (VMSpecifications::SegmentPointerAddresses.count(segment)) {
        int pointerAddr = VMSpecifications::SegmentPointerAddresses.at(segment);
        writeLine("@" + std::to_string(pointerAddr));
        writeLine("A=M");
        writeLine("A=D+A"); 
    } 
}

void VMCodeWriter::writeArithmetic(const std::string& command) {
    if (VMSpecifications::ArithmeticUnaryOperators.count(command)) {
        const std::string& op = VMSpecifications::ArithmeticUnaryOperators.at(command);
        
        writeLine("@SP");
        writeLine("A=M-1");
        writeLine("M=" + op + "M");
        
    } else if (VMSpecifications::ArithmeticBinaryOperators.count(command)) {      
        const std::string& op = VMSpecifications::ArithmeticBinaryOperators.at(command);
        std::string opCommand;
        if (op == "-") {
            opCommand = "M=M-D";
        } else {
            opCommand = "M=D" + op + "M";
        }
        writeSPDecrement();
        writeLine("A=M"); 
        writeLine("D=M"); 
        writeLine("A=A-1");
        writeLine(opCommand);
        
    } else if (VMSpecifications::ArithmeticCompareJumps.count(command)) {
        const std::string& jumpCode = VMSpecifications::ArithmeticCompareJumps.at(command);
        const std::string labelTrue = "COMP_TRUE_" + std::to_string(jumpTarget_);
        const std::string labelEnd = "COMP_END_" + std::to_string(jumpTarget_);
        
        writeSPDecrement();
        writeLine("A=M");
        writeLine("D=M");
        writeLine("A=A-1");
        writeLine("D=M-D"); 
        
        // --- Set Result to True/False ---
        writeLine("@" + labelTrue);
        writeLine("D;" + jumpCode);

        writeLine("@SP");
        writeLine("A=M-1");
        writeLine("M=0"); 
        writeLine("@" + labelEnd);
        writeLine("0;JMP");

        writeLabel(labelTrue);
        writeLine("@SP");
        writeLine("A=M-1");
        writeLine("M=-1");
        writeLabel(labelEnd);
        jumpTarget_++; 
        
    } else {
        std::cerr << "Invalid arithmetic command: " << command << std::endl;
        return;
    }
}

// --- FUNCTION CALLS ---

void VMCodeWriter::writePushDToStack() {
    writeLine("@SP");
    writeLine("A=M");
    writeLine("M=D");
    writeSPIncrement();
}

void VMCodeWriter::writePopToD() {
    writeSPDecrement();
    writeLine("A=M");
    writeLine("D=M");
}


void VMCodeWriter::writeFunction(const std::string& functionName, int nVars) {
    writeLabel(functionName); 
    for (int i = 0; i < nVars; ++i) {
        writeLine("@SP");
        writeLine("A=M");
        writeLine("M=0");
        writeLine("@SP");
        writeLine("M=M+1");
    }
}

void VMCodeWriter::writeCall(const std::string& functionName, int nArgs) {
    std::string returnLabel = functionName + "$ret." + std::to_string(jumpTarget_++);

    writeLine("@" + returnLabel);
    writeLine("D=A");
    writePushDToStack();

    const std::vector<std::string> segments = {"LCL", "ARG", "THIS", "THAT"};
    for (const auto& segment : segments) {
        writeLine("@" + segment);
        writeLine("D=M");
        writePushDToStack();
    }

    writeLine("@SP");
    writeLine("D=M"); 
    writeLine("@" + std::to_string(nArgs + 5));
    writeLine("D=D-A");
    writeLine("@ARG");
    writeLine("M=D");

    writeLine("@SP");
    writeLine("D=M");
    writeLine("@LCL");
    writeLine("M=D");

    writeGoTo(functionName); 

    writeLabel(returnLabel);
}

void VMCodeWriter::writeReturn() {
    writeLine("@LCL");
    writeLine("D=M");
    writeLine("@R13"); 
    writeLine("M=D");

    writeLine("@5");
    writeLine("A=D-A"); 
    writeLine("D=M");
    writeLine("@R14"); 
    writeLine("M=D");

    writePopToD();
    writeLine("@ARG");
    writeLine("A=M");
    writeLine("M=D"); 

    writeLine("@ARG");
    writeLine("D=M+1");
    writeLine("@SP");
    writeLine("M=D"); 

    const std::vector<std::string> segments = {"THAT", "THIS", "ARG", "LCL"};
    int offset = 1; 
    for (const auto& segment : segments) {
        writeLine("@R13");
        writeLine("D=M");           
        writeLine("@" + std::to_string(offset));
        writeLine("A=D-A");
        writeLine("D=M");
        writeLine("@" + segment); 
        writeLine("M=D"); 
        offset++;
    }
    writeLine("@R14");
    writeLine("A=M");
    writeLine("0;JMP");
}