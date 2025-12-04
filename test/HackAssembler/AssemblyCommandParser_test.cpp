#include <catch2/catch_test_macros.hpp>
#include <optional>
#include "HackAssembler/AssemblyCommandParser.hpp" 

using CommandType = AssemblyCommandParser::CommandType;

TEST_CASE("CommandParser correctly identifies command types", "[CommandParser][type]") {
    
    SECTION("A-Instruction identification") {
        AssemblyCommandParser p1("@R0");
        REQUIRE(p1.type() == CommandType::A_INSTRUCTION);

        AssemblyCommandParser p2("@LOOP_START // A-command with comment");
        REQUIRE(p2.type() == CommandType::A_INSTRUCTION);
    }

    SECTION("L-Instruction identification") {
        AssemblyCommandParser p1("(LOOP)");
        REQUIRE(p1.type() == CommandType::L_INSTRUCTION);

        AssemblyCommandParser p2("(END) // End of program loop");
        REQUIRE(p2.type() == CommandType::L_INSTRUCTION);
    }
    
    SECTION("C-Instruction identification") {
        AssemblyCommandParser p1("D=M+1");
        REQUIRE(p1.type() == CommandType::C_INSTRUCTION);

        AssemblyCommandParser p2("0;JMP");
        REQUIRE(p2.type() == CommandType::C_INSTRUCTION);
        
        AssemblyCommandParser p3("D=D+M // Comment");
        REQUIRE(p3.type() == CommandType::C_INSTRUCTION);
    }

    SECTION("NO_COMMAND identification (Whitespace & Comments)") {
        AssemblyCommandParser p1("// Just a comment");
        REQUIRE(p1.type() == CommandType::NO_COMMAND);

        AssemblyCommandParser p2("   "); // Only whitespace
        REQUIRE(p2.type() == CommandType::NO_COMMAND);

        AssemblyCommandParser p3(""); // Empty string
        REQUIRE(p3.type() == CommandType::NO_COMMAND);
    }
}

TEST_CASE("CommandParser correctly extracts all fields", "[CommandParser][fields]") {
    
    SECTION("Symbol extraction for A and L commands") {
        AssemblyCommandParser p_a("@100");
        REQUIRE(p_a.symbol() == "100");

        AssemblyCommandParser p_label("(MY_LABEL)");
        REQUIRE(p_label.symbol() == "MY_LABEL");
        
        AssemblyCommandParser p_var("@counter");
        REQUIRE(p_var.symbol() == "counter");
    }
    
    SECTION("Extraction of full C-Command (D=A;JMP)") {
        AssemblyCommandParser p("AMD=D+M;JGT");
        REQUIRE(p.dest() == "AMD");
        REQUIRE(p.comp() == "D+M");
        REQUIRE(p.jump() == "JGT");
    }

    SECTION("Extraction of Comp;Jump (No Dest)") {
        AssemblyCommandParser p("M-1;JLE");
        REQUIRE(p.dest() == "");
        REQUIRE(p.comp() == "M-1");
        REQUIRE(p.jump() == "JLE");
    }

    SECTION("Extraction of Dest=Comp (No Jump)") {
        AssemblyCommandParser p("A=!M");
        REQUIRE(p.dest() == "A");
        REQUIRE(p.comp() == "!M");
        REQUIRE(p.jump() == "");
    }
    
    SECTION("Extraction of Comp Only") {
        AssemblyCommandParser p("D&A");
        REQUIRE(p.dest() == "");
        REQUIRE(p.comp() == "D&A");
        REQUIRE(p.jump() == "");
    }
}

TEST_CASE("CommandParser is robust to whitespace and comments", "[CommandParser][robustness]") {
    SECTION("Complex line with all elements") {
        AssemblyCommandParser p("@VAR_01 // Assign variable");
        REQUIRE(p.type() == CommandType::A_INSTRUCTION);
        REQUIRE(p.symbol() == "VAR_01");
    }
}