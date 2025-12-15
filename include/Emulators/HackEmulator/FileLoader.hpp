#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <bitset>
#include <cstdint>

class FileLoader {
private:
    int16_t convertBinaryStringToSignedInt16(const std::string& binary_string);
    std::vector<int16_t> loadHackFile(const std::string& filepath);
    std::vector<int16_t> loadBinFile(const std::string& filepath);

public:
    std::vector<int16_t> loadFile(const std::string& filepath);
};

#endif