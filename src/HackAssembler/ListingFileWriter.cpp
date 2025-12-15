#include "HackAssembler/ListingFileWriter.hpp"

// Helper to center text for formatting (moved from HackAssembler)
std::string ListingFileWriter::centerSpaces(const std::string& s) {
    int repeat = listingSpacing_ - static_cast<int>(s.length());
    if (repeat <= 0) {
        return s;
    }
    
    int left, right;
    if (repeat % 2 == 1) {
        left = repeat / 2;
        right = repeat / 2 + 1;
    } else {
        left = repeat / 2;
        right = repeat / 2;
    }
    return std::string(left, ' ') + s + std::string(right, ' ');
}

void ListingFileWriter::writeHeader() {
    writer_ << centerSpaces("ROM") << "|"
           << centerSpaces("Address") << "| Source\n";
}

void ListingFileWriter::writeGeneric(const std::string& rawLine) {
    writer_ << std::string(listingSpacing_, ' ') << "|"
            << std::string(listingSpacing_, ' ') << "| "
            << rawLine << "\n";
}

void ListingFileWriter::writeVarAddress(int romLineNo, int ramAddress, const std::string& rawLine) {
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces("RAM[" + std::to_string(ramAddress) + "]") + "| "
                              + rawLine;
    writer_ << lineToWrite << "\n";
}

void ListingFileWriter::writeConstant(int romLineNo, const std::string& constant, const std::string& rawLine) {
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces(constant) + "| "
                              + rawLine;
    writer_ << lineToWrite << "\n";
}

void ListingFileWriter::writeCommand(int romLineNo, const std::string& rawLine) {
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces("") + "| " 
                              + rawLine;
    writer_ << lineToWrite << "\n";
}

void ListingFileWriter::writeLabel(int romLineNo, const std::string& label, const std::string& rawLine) {
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces("ROM[" + std::to_string(romLineNo) + "]") +
                              "| " + rawLine;
    writer_ << lineToWrite << "\n";
}