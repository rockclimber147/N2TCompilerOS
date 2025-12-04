#include <catch2/catch_test_macros.hpp>
#include "HackAssembler/AssemblyCommandParser.hpp"

TEST_CASE("A-Instruction is correctly identified", "[parser][A_CMD]") {
    AssemblyCommandParser parser("@2112");
    // Assert that the command type is A_INSTRUCTION
    REQUIRE(parser.type() == AssemblyCommandParser::A_INSTRUCTION); 
    // Assert that the symbol is "2112"
    REQUIRE(parser.symbol() == "2112"); 
}