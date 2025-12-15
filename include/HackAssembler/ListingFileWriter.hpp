#ifndef LISTING_FILE_WRITER_HPP
#define LISTING_FILE_WRITER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class ListingFileWriter {
public:
    // Constructor opens the output stream
    ListingFileWriter(const std::string& filePath) : listingSpacing_(10) {
        writer_.open(filePath);
        if (!writer_.is_open()) {
            throw std::runtime_error("Could not open output listing file at " + filePath);
        }
    }

    ~ListingFileWriter() {
        if (writer_.is_open()) {
            writer_.close();
        }
    }

    void writeHeader();
    void writeGeneric(const std::string& rawLine);
    void writeVarAddress(int romLineNo, int ramAddress, const std::string& rawLine);
    void writeConstant(int romLineNo, const std::string& constant, const std::string& rawLine);
    void writeCommand(int romLineNo, const std::string& rawLine);
    void writeLabel(int romLineNo, const std::string& label, const std::string& rawLine);
    
private:
    std::ofstream writer_;
    const int listingSpacing_;

    // Helper to center text for formatting
    std::string centerSpaces(const std::string& s);
};

#endif // LISTING_FILE_WRITER_HPP