#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include "HackAssembler/SymbolTable.hpp"

SymbolTable table = SymbolTable();

TEST_CASE("SymbolTable throws correct errors on invalid input") {
    std::string label = "JUMP";
    table.addJumpLabel(label, 10);
    REQUIRE_THROWS_AS(table.addJumpLabel(label, 10), std::invalid_argument);
    REQUIRE_THROWS_AS(table.addJumpLabel("JUMP2", -10), std::invalid_argument);
}