#include <catch2/catch_test_macros.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <cstdint>
#include <stdexcept>

#include "Emulators/HackEmulator/FileLoader.hpp" 

const std::string TEST_HACK_PATH = "../test/Emulators/HackEmulator/TestFiles/test.hack";
const std::string TEST_BIN_PATH  = "../test/Emulators/HackEmulator/TestFiles/test.bin";

// The content of the test.hack file you provided:
const std::string HACK_CONTENT = R"(
0000000000000000
1111111111111111
1010101010101010
1111000011110000
1100110011001100
1111111100000000
)";

// --- Utility Functions for Testing ---

namespace TestUtils {

    /**
     * @brief Converts a 16-character string of '1's and '0's into a signed 16-bit integer.
     */
    int16_t convertBinaryStringToSignedInt16(const std::string& binary_string) {
        std::bitset<16> bits(binary_string); 
        return static_cast<int16_t>(bits.to_ulong()); 
    }

    /**
     * @brief Utility to read a .hack file (text-based binary strings) and write a corresponding 
     * .bin file (raw 16-bit integers) for testing purposes.
     */
    void createBinFromHack(const std::string& input_hack_file, const std::string& output_bin_file) {
        // Ensure the directory exists (optional, but good for robustness)
        // Note: For simplicity, we are assuming the directory structure for TEST_BIN_PATH already exists.

        std::ifstream input_file(input_hack_file);
        std::ofstream output_file(output_bin_file, std::ios::binary);

        if (!input_file.is_open() || !output_file.is_open()) {
            throw std::runtime_error("Error opening input or output file in createBinFromHack utility.");
        }

        std::string line;
        while (std::getline(input_file, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            
            if (line.empty() || line.front() == '/') {
                continue;
            }
            
            if (line.length() != 16) {
                 throw std::runtime_error("Invalid instruction length in .hack file for utility: " + line);
            }

            try {
                int16_t instruction = convertBinaryStringToSignedInt16(line);
                output_file.write(reinterpret_cast<const char*>(&instruction), sizeof(instruction));
                
            } catch (const std::invalid_argument& e) {
                throw std::runtime_error("Invalid character in .hack instruction for utility: " + line);
            }
        }
        
        if (output_file.fail()) {
            throw std::runtime_error("Error during writing to .bin file.");
        }
    }
}


TEST_CASE("FileLoader loads files correctly", "[FileLoader][type]") {
    const std::vector<int16_t> EXPECTED_VALUES = {
        0,           // 0000000000000000
        -1,          // 1111111111111111
        -21846,      // 1010101010101010
        -3856,       // 1111000011110000
        -13108,      // 1100110011001100
        -256         // 1111111100000000
    };

    std::ofstream hack_out(TEST_HACK_PATH);
    REQUIRE(hack_out.is_open());
    hack_out << HACK_CONTENT;
    hack_out.close();

    REQUIRE_NOTHROW(TestUtils::createBinFromHack(TEST_HACK_PATH, TEST_BIN_PATH));
    FileLoader loader;

    SECTION("Loads .hack file correctly") {
        std::vector<int16_t> instructions = loader.loadFile(TEST_HACK_PATH);
        
        REQUIRE(instructions.size() == EXPECTED_VALUES.size());
        REQUIRE(instructions == EXPECTED_VALUES);
    }

    SECTION("Loads .bin file correctly") {
        std::vector<int16_t> instructions = loader.loadFile(TEST_BIN_PATH);

        REQUIRE(instructions.size() == EXPECTED_VALUES.size());
        REQUIRE(instructions == EXPECTED_VALUES);
    }

    SECTION("Throws exception for unknown file type") {
        REQUIRE_THROWS_AS(loader.loadFile("invalid.ext"), std::runtime_error);
    }
    std::remove(TEST_BIN_PATH.c_str());
}