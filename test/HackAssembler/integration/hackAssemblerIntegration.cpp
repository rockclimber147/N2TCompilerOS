#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "HackAssembler/HackAssembler.hpp"

namespace fs = std::filesystem;

std::string trim(const std::string& str) {
    const size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return "";
    }
    const size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void compareFiles(const std::string& generatedOutputPath, const std::string& expectedOutputPath) {
    std::ifstream generatedFile(generatedOutputPath);
    if (!generatedFile.is_open()) {
        throw std::runtime_error("Could not open generated file: " + generatedOutputPath);
    }

    std::ifstream expectedFile(expectedOutputPath);
    if (!expectedFile.is_open()) {
        throw std::runtime_error("Could not open expected file: " + expectedOutputPath);
    }

    std::string generatedLine;
    std::string expectedLine;
    int lineNumber = 0;

    while (true) {
        bool generatedHasLine = static_cast<bool>(std::getline(generatedFile, generatedLine));
        bool expectedHasLine = static_cast<bool>(std::getline(expectedFile, expectedLine));
        lineNumber++;

        if (!generatedHasLine && !expectedHasLine) {
            return;
        }
        if (!generatedHasLine && expectedHasLine) {
            throw std::runtime_error(
                "File comparison failed: Generated file is too short. "
                "Missing line at expected line " + std::to_string(lineNumber)
            );
        }
        if (generatedHasLine && !expectedHasLine) {
            throw std::runtime_error(
                "File comparison failed: Generated file has extra content. "
                "Extra line found at line " + std::to_string(lineNumber)
            );
        }
        if (trim(generatedLine) != trim(expectedLine)) {
            throw std::runtime_error(
                "File comparison failed at line " + std::to_string(lineNumber) + 
                ".\n  Expected: \"" + expectedLine + "\"" +
                "\n  Actual:   \"" + generatedLine + "\""
            );
        }
    }
}

void run_test_case(const std::string& folder, const std::string& testName) {
    const fs::path TEST_FILE_PATH = __FILE__;
    const fs::path BASE_DIR = TEST_FILE_PATH.parent_path().parent_path().parent_path().parent_path(); 
    const fs::path TEST_ROOT = BASE_DIR / "test/HackAssembler/integration";

    const fs::path INPUT_DIR = TEST_ROOT / "input" / folder;
    const fs::path EXPECTED_DIR = TEST_ROOT / "expectedOutput" / folder;
    const fs::path GENERATED_DIR = TEST_ROOT / "generatedOutput" / folder;

    const std::string assemblyFile = testName + ".asm";
    const std::string hackFile = testName + ".hack";

    // 3. Ensure Generated Output Directory Exists
    fs::create_directories(GENERATED_DIR);
    HackAssembler assembler = HackAssembler(
        testName,
        INPUT_DIR.string() + "/",
        GENERATED_DIR.string() + "/",
        false
    );

    assembler.assemble();
    assembler.closeWriters();


    const fs::path actualPath = GENERATED_DIR / hackFile;
    const fs::path expectedPath = EXPECTED_DIR / hackFile;
    REQUIRE_NOTHROW(compareFiles(actualPath.string(), expectedPath.string()));
}

TEST_CASE("Assembler Integration Tests", "[Assembler][Integration][Files]") {
    
    SECTION("Test Add.asm (Basic Arithmetic)") {
        run_test_case("add", "Add"); 
    }

    SECTION("Test Max.asm (Basic Arithmetic)") {
        run_test_case("max", "MaxL"); 
    }
    SECTION("Test MaxL.asm (Basic Arithmetic)") {
        run_test_case("max", "MaxL"); 
    }

    SECTION("Test Rect.asm (Draw a Rectangle)") {
        run_test_case("rect", "Rect"); 
    }
    SECTION("Test RectL.asm (Draw a Rectangle)") {
        run_test_case("rect", "RectL"); 
    }

    SECTION("Test Pong.asm (Draw a Rectangle)") {
        run_test_case("pong", "Pong"); 
    }
    SECTION("Test PongL.asm (Draw a Rectangle)") {
        run_test_case("pong", "PongL"); 
    }
}
