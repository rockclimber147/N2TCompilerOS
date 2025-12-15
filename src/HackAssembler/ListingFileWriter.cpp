#include "HackAssembler/ListingFileWriter.hpp"


ListingFileWriter::ListingFileWriter(const std::string& filePath, bool shouldGenerate) 
    : isActive_(shouldGenerate) 
{
    if (isActive_) {
        writer_.open(filePath); 
        if (!writer_.is_open()) {
            throw std::runtime_error("Could not open output listing file at " + filePath);
        }
    }
}
ListingFileWriter::~ListingFileWriter() {
}

void ListingFileWriter::writeHeader() {
    if (!isActive_) return;
    writer_ << centerSpaces("ROM") << "|"
           << centerSpaces("Address") << "| Source\n";
}

void ListingFileWriter::writeGeneric(const std::string& rawLine) {
    if (!isActive_) return;
    writer_ << std::string(listingSpacing_, ' ') << "|"
            << std::string(listingSpacing_, ' ') << "| "
            << rawLine << "\n";
}

void ListingFileWriter::writeVarAddress(int romLineNo, int ramAddress, const std::string& rawLine) {
    if (!isActive_) return;
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces("RAM[" + std::to_string(ramAddress) + "]") + "| "
                              + rawLine;
    writer_ << lineToWrite << "\n";
}

void ListingFileWriter::writeConstant(int romLineNo, const std::string& constant, const std::string& rawLine) {
    if (!isActive_) return;
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces(constant) + "| "
                              + rawLine;
    writer_ << lineToWrite << "\n";
}

void ListingFileWriter::writeCommand(int romLineNo, const std::string& rawLine) {
    if (!isActive_) return;
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces("") + "| " 
                              + rawLine;
    writer_ << lineToWrite << "\n";
}

void ListingFileWriter::writeLabel(int romLineNo, const std::string& label, const std::string& rawLine) {
    if (!isActive_) return;
    std::string lineToWrite = centerSpaces(std::to_string(romLineNo)) + "|" +
                              centerSpaces("ROM[" + std::to_string(romLineNo) + "]") +
                              "| " + rawLine;
    writer_ << lineToWrite << "\n";
}

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
