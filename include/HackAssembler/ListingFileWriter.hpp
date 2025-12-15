#ifndef LISTING_FILE_WRITER_HPP
#define LISTING_FILE_WRITER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class ListingFileWriter {
public:
    // New constructor signature includes the conditional flag
    ListingFileWriter(const std::string& filePath, bool shouldGenerate);
    
    // Destructor remains the same (closes file if open)
    ~ListingFileWriter();

    // All public methods remain the same (no 'override' needed)
    void writeHeader();
    void writeGeneric(const std::string& rawLine);
    void writeVarAddress(int romLineNo, int ramAddress, const std::string& rawLine);
    void writeConstant(int romLineNo, const std::string& constant, const std::string& rawLine);
    void writeCommand(int romLineNo, const std::string& rawLine);
    void writeLabel(int romLineNo, const std::string& label, const std::string& rawLine);
    
private:
    std::ofstream writer_;
    const int listingSpacing_ = 10;
    bool isActive_ = false; // New flag to track if writing should occur

    // Helper to center text for formatting
    std::string centerSpaces(const std::string& s);
    
    // New helper to perform the conditional check
    inline bool shouldWrite() const { return isActive_ && writer_.is_open(); }
};

#endif // LISTING_FILE_WRITER_HPP