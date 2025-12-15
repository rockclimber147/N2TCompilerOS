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

// --- BOOTSTRAP (writeInit) ---

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
    writeLine("@0\n");
    writeLine("AM=M-1\n");
    writeLine("D=M\n");
    writeLine("@" + label + "\n");
    writeLine("D;JNE\n");
}

void VMCodeWriter::writePush(const std::string& segment, int index) {
    writeAsComment("push " + segment + " " + std::to_string(index));

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
    writeAsComment("pop " + segment + " " + std::to_string(index));
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
        writeLine("A=A+D"); 
    } 
}

void VMCodeWriter::writeArithmetic(const std::string& command) {
    writeAsComment(command); // Write the original VM command as a comment

    if (VMSpecifications::ArithmeticUnaryOperators.count(command)) {
        const std::string& op = VMSpecifications::ArithmeticUnaryOperators.at(command);
        
        writeLine("@SP");
        writeLine("A=M-1");
        writeLine("M=" + op + "M");
        
    } else if (VMSpecifications::ArithmeticBinaryOperators.count(command)) {      
        const std::string& op = VMSpecifications::ArithmeticBinaryOperators.at(command);
        
        writeSPDecrement();
        writeLine("A=M"); 
        writeLine("D=M"); 
        writeLine("A=A-1");
        writeLine("M=M" + op + "D");
        
    } else if (VMSpecifications::ArithmeticCompareJumps.count(command)) {
        const std::string& jumpCode = VMSpecifications::ArithmeticCompareJumps.at(command);
        const std::string labelTrue = "COMP_TRUE_" + std::to_string(jumpTarget_);
        const std::string labelEnd = "COMP_END_" + std::to_string(jumpTarget_);
        
        writeSPDecrement();
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

void VMCodeWriter::writeFunction(const std::string& functionName, int nVars) {
    // TODO: Implement function label and local variable initialization loop.
}

void VMCodeWriter::writeCall(const std::string& functionName, int nArgs) {
    // This public method simply calls the implementation.
    writeCallImpl(functionName, nArgs);
}

void VMCodeWriter::writeCallImpl(const std::string& functionName, int nArgs) {
    // TODO: Implement the 5-step call sequence:
    // 1. push return-address
    // 2. push LCL, ARG, THIS, THAT
    // 3. ARG = SP - nArgs - 5
    // 4. LCL = SP
    // 5. goto functionName
}

void VMCodeWriter::writeReturn() {
    // TODO: Implement the 8-step return sequence.
}