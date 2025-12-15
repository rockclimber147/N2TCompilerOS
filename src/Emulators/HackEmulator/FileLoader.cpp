#include "Emulators/HackEmulator/FileLoader.hpp"

int16_t FileLoader::convertBinaryStringToSignedInt16(const std::string& binary_string) {
    std::bitset<16> bits(binary_string); 
    return static_cast<int16_t>(bits.to_ulong()); 
}

std::vector<int16_t> FileLoader::loadHackFile(const std::string& filepath) {
    std::vector<int16_t> instructions;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open .hack file: " + filepath);
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        
        if (line.empty()) {
            continue;
        }
        if (line.length() != 16) {
            throw std::runtime_error("Invalid instruction length in .hack file. Expected 16 bits, got " + std::to_string(line.length()));
        }

        try {
            instructions.push_back(convertBinaryStringToSignedInt16(line));
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid character in .hack instruction: " + line);
        }
    }

    return instructions;
}

std::vector<int16_t> FileLoader::loadBinFile(const std::string& filepath){
    std::vector<int16_t> instructions;
    std::ifstream file(filepath, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open .bin file: " + filepath);
    }
    int16_t instruction;
    while (file.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) {
        instructions.push_back(instruction);
    }
    
    if (!file.eof() && file.fail()) {
        throw std::runtime_error("Read error or truncated instruction in .bin file.");
    }

    return instructions;
}

std::vector<int16_t> FileLoader::loadFile(const std::string& filepath) {
    if (filepath.rfind(".hack") != std::string::npos) {
        return loadHackFile(filepath);
    } else if (filepath.rfind(".bin") != std::string::npos) {
        return loadBinFile(filepath);
    } else {
        throw (std::runtime_error("Can only load .hack and .bin files, unknown extension for " + filepath));
    }
}
