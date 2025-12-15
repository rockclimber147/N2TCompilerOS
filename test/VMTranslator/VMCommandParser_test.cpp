#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <stdexcept>
#include "VMTranslator/VMCommandParser.hpp" 
#include "VMTranslator/VMSpecifications.hpp"

using CommandType = VMSpecifications::CommandType;

// --- Test Group 1: Command Type Identification ---

TEST_CASE("VMCommandParser correctly identifies command types and cleans input", "[CommandParser][type]") {
    
    // --- C_PUSH / C_POP ---
    SECTION ("C_PUSH identification (standard)") {
        VMCommandParser vmp("push local 10");
        REQUIRE(vmp.type() == CommandType::C_PUSH);
        REQUIRE(vmp.getCleanCommand() == "push local 10");
    }

    SECTION ("C_POP identification (with trailing whitespace)") {
        VMCommandParser vmp(" pop pointer 0 \t");
        REQUIRE(vmp.type() == CommandType::C_POP);
        REQUIRE(vmp.getCleanCommand() == "pop pointer 0");
    }
    
    SECTION ("C_PUSH identification (with comment)") {
        VMCommandParser vmp("push this 5 // segment index");
        REQUIRE(vmp.type() == CommandType::C_PUSH);
        REQUIRE(vmp.getCleanCommand() == "push this 5");
    }

    // --- C_ARITHMETIC ---
    SECTION ("C_ARITHMETIC identification (unary)") {
        VMCommandParser vmp("neg");
        REQUIRE(vmp.type() == CommandType::C_ARITHMETIC);
    }

    SECTION ("C_ARITHMETIC identification (binary)") {
        VMCommandParser vmp("sub \t");
        REQUIRE(vmp.type() == CommandType::C_ARITHMETIC);
    }
    
    SECTION ("C_ARITHMETIC identification (comparison)") {
        VMCommandParser vmp("gt // greater than");
        REQUIRE(vmp.type() == CommandType::C_ARITHMETIC);
    }

    // --- Flow Control ---
    SECTION ("C_LABEL identification") {
        VMCommandParser vmp("label LOOP_START");
        REQUIRE(vmp.type() == CommandType::C_LABEL);
    }
    
    SECTION ("C_GOTO identification") {
        VMCommandParser vmp("goto MAIN_LOOP");
        REQUIRE(vmp.type() == CommandType::C_GOTO);
    }
    
    SECTION ("C_IF_GOTO identification") {
        VMCommandParser vmp("if-goto EXIT_COND");
        REQUIRE(vmp.type() == CommandType::C_IF_GOTO);
    }
    
    // --- Function Commands ---
    SECTION ("C_FUNCTION identification") {
        VMCommandParser vmp("function Main.fib 3");
        REQUIRE(vmp.type() == CommandType::C_FUNCTION);
    }
    
    SECTION ("C_CALL identification") {
        VMCommandParser vmp("call Sys.init 0");
        REQUIRE(vmp.type() == CommandType::C_CALL);
    }

    SECTION ("C_RETURN identification") {
        VMCommandParser vmp("return");
        REQUIRE(vmp.type() == CommandType::C_RETURN);
    }
}

// --- Test Group 2: Argument Extraction ---

TEST_CASE("VMCommandParser correctly extracts arguments (arg1 and arg2)", "[CommandParser][args]") {

    // --- C_PUSH / C_POP (Require arg1 and arg2) ---
    SECTION("C_PUSH argument extraction") {
        VMCommandParser vmp("push static 17");
        REQUIRE(vmp.type() == CommandType::C_PUSH);
        REQUIRE(vmp.arg1() == "static");
        REQUIRE(vmp.arg2() == 17);
    }
    
    SECTION("C_POP argument extraction") {
        VMCommandParser vmp("pop argument 4");
        REQUIRE(vmp.type() == CommandType::C_POP);
        REQUIRE(vmp.arg1() == "argument");
        REQUIRE(vmp.arg2() == 4);
    }

    // --- C_ARITHMETIC (Requires arg1 only, which is the command itself) ---
    SECTION("C_ARITHMETIC argument extraction") {
        VMCommandParser vmp("and");
        REQUIRE(vmp.type() == CommandType::C_ARITHMETIC);
        REQUIRE(vmp.arg1() == "and"); // arg1 is the operation word
        REQUIRE_THROWS_AS(vmp.arg2(), std::runtime_error); // Should throw for arg2
    }

    // --- C_LABEL / C_GOTO / C_IF_GOTO (Require arg1 only, which is the label) ---
    SECTION("C_GOTO argument extraction") {
        VMCommandParser vmp("goto MAIN_LOOP_0");
        REQUIRE(vmp.type() == CommandType::C_GOTO);
        REQUIRE(vmp.arg1() == "MAIN_LOOP_0"); // arg1 is the label
        REQUIRE_THROWS_AS(vmp.arg2(), std::runtime_error); 
    }
    
    // --- C_FUNCTION / C_CALL (Require arg1 and arg2) ---
    SECTION("C_FUNCTION argument extraction") {
        VMCommandParser vmp("function Class.func 5");
        REQUIRE(vmp.type() == CommandType::C_FUNCTION);
        REQUIRE(vmp.arg1() == "Class.func");
        REQUIRE(vmp.arg2() == 5);
    }
    
    SECTION("C_RETURN arguments") {
        VMCommandParser vmp("return");
        REQUIRE(vmp.type() == CommandType::C_RETURN);
        REQUIRE_THROWS_AS(vmp.arg1(), std::runtime_error); // Should throw for arg1
        REQUIRE_THROWS_AS(vmp.arg2(), std::runtime_error); // Should throw for arg2
    }
}

// --- Test Group 3: Edge Cases and Errors ---

TEST_CASE("VMCommandParser handles invalid input and edge cases", "[CommandParser][errors]") {
    
    // --- Invalid Commands and Empty Lines ---
    SECTION("Empty line input") {
        VMCommandParser vmp("");
        REQUIRE(vmp.isValidCommand() == false);
        REQUIRE(vmp.type() == CommandType::C_INVALID);
        REQUIRE(vmp.getCleanCommand().empty());
    }

    SECTION("Comment-only line") {
        VMCommandParser vmp("   // This is only a comment \t");
        REQUIRE(vmp.isValidCommand() == false);
        REQUIRE(vmp.type() == CommandType::C_INVALID);
        REQUIRE(vmp.getCleanCommand().empty());
    }
    
    SECTION("Invalid command word") {
        VMCommandParser vmp("pushing local 10");
        REQUIRE(vmp.isValidCommand() == false);
        REQUIRE(vmp.type() == CommandType::C_INVALID);
    }

    // --- Argument Errors ---
    SECTION("Missing arg2 for C_PUSH") {
        VMCommandParser vmp("push local");
        REQUIRE(vmp.type() == CommandType::C_PUSH);
        REQUIRE(vmp.arg1() == "local");
        // Should throw because arg2 is expected (3 tokens total)
        REQUIRE_THROWS_AS(vmp.arg2(), std::runtime_error); 
    }

    SECTION("Invalid arg2 format") {
        VMCommandParser vmp("call MyFunc X");
        REQUIRE(vmp.type() == CommandType::C_CALL);
        REQUIRE(vmp.arg1() == "MyFunc");
        REQUIRE_THROWS_AS(vmp.arg2(), std::runtime_error);
    }
}