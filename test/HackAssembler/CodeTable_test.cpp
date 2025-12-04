#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include "HackAssembler/CodeTable.hpp"


TEST_CASE("CodeTable lookups (dest, comp, jump) return correct binary codes", "[CodeTable][lookup]") {
    SECTION("Dest (D) field lookups are correct") {
        REQUIRE(CodeTable::dest("M") == "001");
        REQUIRE(CodeTable::dest("AMD") == "111");
        REQUIRE(CodeTable::dest("") == "000");
    }

    SECTION("Jump (J) field lookups are correct") {
        REQUIRE(CodeTable::jump("JMP") == "111");
        REQUIRE(CodeTable::jump("JEQ") == "010");
        REQUIRE(CodeTable::jump("") == "000");
    }

    SECTION("Comp (C) field lookups are correct") {
        REQUIRE(CodeTable::comp("D+A") == "0000010");
        REQUIRE(CodeTable::comp("!M") == "1110001");
        REQUIRE(CodeTable::comp("0") == "0101010");
    }

    SECTION("Swapped operators give identical output on idempotent operations") {
        REQUIRE(CodeTable::comp("M|D") == CodeTable::comp("D|M"));
        REQUIRE(CodeTable::comp("A|D") == CodeTable::comp("D|A"));
        REQUIRE(CodeTable::comp("M&D") == CodeTable::comp("D&M"));
        REQUIRE(CodeTable::comp("A&D") == CodeTable::comp("D&A"));
    }

    SECTION("Invalid mnemonics throw an out_of_range exception") {
        REQUIRE_THROWS_AS(CodeTable::comp("D+Q"), std::out_of_range);
        REQUIRE_THROWS_AS(CodeTable::dest("BAD"), std::out_of_range);
        REQUIRE_THROWS_AS(CodeTable::jump("JXX"), std::out_of_range);
    }
}