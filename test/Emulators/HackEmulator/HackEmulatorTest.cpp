#include <catch2/catch_test_macros.hpp>
#include "Emulators/HackEmulator/FileLoader.hpp" // Required for FileLoader (if used in other tests)
#include "Emulators/HackEmulator/HackEmulator.hpp" // The class under test
#include <vector>
#include <cstdint>

// Utility to create A-instructions (e.g., @100)
int16_t create_A_instruction(uint16_t value) {
    // A-Instruction has MSB=0, value in lower 15 bits
    return static_cast<int16_t>(value & 0x7FFF);
}

// Utility to create C-instructions (comp=dest;jump)
int16_t create_C_instruction(uint8_t comp_code, uint8_t dest_code, uint8_t jump_code) {
    // C-Instruction format: 111 a cccccc ddd jjj
    // Prefix 111 is 0xE000
    int16_t instruction = 0xE000;
    
    // Comp (6 bits, c) -> bits 6-11
    instruction |= (comp_code << 6);
    
    // Dest (3 bits, d) -> bits 3-5
    instruction |= (dest_code << 3);
    
    // Jump (3 bits, j) -> bits 0-2
    instruction |= jump_code;
    
    return instruction;
}


TEST_CASE("HackEmulator: Core Register and ALU Operations", "[HackEmulator][Core][ALU]") {
    HackEmulator emu;

    // --- SETUP: Initialize a known state ---
    emu.setRamValue(10, 500); // Set RAM[10] to 500
    emu.setRamValue(3, 10);   // Set RAM[THIS_BASE_ADDR] (address 3) to 10
    emu.setRamValue(4, 20);   // Set RAM[THAT_BASE_ADDR] (address 4) to 20
    
    // Setup initial register values
    emu.setARegister(10);
    emu.setDRegister(5);
    
    // --- Instruction Vector (Program) ---
    std::vector<int16_t> program = {
        // Line 0: @100 (A=100)
        create_A_instruction(100), 
        // Line 1: D=A (D=100)
        create_C_instruction(0b110000, 0b010, 0b000), 
        // Line 2: M=D&A (M[100]=100 & 100 = 100)
        create_C_instruction(0b000000, 0b001, 0b000),
        // Line 3: D=M+1 (A=100, M=100, D=101)
        create_C_instruction(0b110111, 0b010, 0b000), 
        // Line 4: D=D-A (D=101-100 = 1)
        create_C_instruction(0b010011, 0b010, 0b000), 
        // Line 5: M=0 (A=100, M[100]=0)
        create_C_instruction(0b101010, 0b001, 0b000) 
    };
    
    // --- ACT: Load and Run ---
    emu.loadProgram(program);
    
    // 1. Line 0: @100
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 100);
    REQUIRE(emu.getPC() == 1);

    // 2. Line 1: D=A (A=100, D=100)
    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 100);
    REQUIRE(emu.getARegister() == 100);
    REQUIRE(emu.getPC() == 2);

    // 3. Line 2: M=D&A (A=100, D=100. Result=100. M[100]=100)
    emu.executeNextInstruction();
    REQUIRE(emu.getM() == 100);
    REQUIRE(emu.getPC() == 3);

    // 4. Line 3: D=M+1 (A=100, M=100. Result=101. D=101)
    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 101);
    REQUIRE(emu.getPC() == 4);

    // 5. Line 4: D=D-A (A=100, D=101. Result=1. D=1)
    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 1);
    REQUIRE(emu.getPC() == 5);
    
    // 6. Line 5: M=0 (A=100. Result=0. M[100]=0)
    emu.executeNextInstruction();
    REQUIRE(emu.getM() == 0);
    REQUIRE(emu.getPC() == 6);
}

TEST_CASE("HackEmulator: Segment Accessors and Pointers", "[HackEmulator][Memory]") {
    HackEmulator emu;
    
    // Setup the Pointer Registers
    emu.setRamValue(HackEmulator::THIS_POINTER, 500); // RAM[3] points to address 500
    emu.setRamValue(HackEmulator::THAT_POINTER, 600); // RAM[4] points to address 600
    
    // Set specific segment values
    emu.setRamValue(HackEmulator::STATIC_BASE_ADDR + 2, 123); // Static[2] = 123 (RAM[18])
    emu.setRamValue(500 + 5, 456);                           // This[5] = 456 (RAM[505])
    emu.setRamValue(600 + 1, 789);                           // That[1] = 789 (RAM[601])

    SECTION("getStatic returns correct value") {
        REQUIRE(emu.getStatic(2) == 123);
        REQUIRE(emu.getStatic(0) == 0); // Uninitialized value
    }

    SECTION("getThis returns correct value using RAM pointer") {
        REQUIRE(emu.getThis(5) == 456);
    }
    
    SECTION("getThat returns correct value using RAM pointer") {
        REQUIRE(emu.getThat(1) == 789);
    }
    
    SECTION("setRamValue and getM validation") {
        emu.setARegister(1000);
        emu.setRamValue(1000, -99);
        REQUIRE(emu.getM() == -99);
    }
}


TEST_CASE("HackEmulator: Conditional and Unconditional Jumps", "[HackEmulator][Jump]") {
    HackEmulator emu;
    
    // Target address for all jumps is 100.
    std::vector<int16_t> program = {
        // Line 0: @100 (A=100)
        create_A_instruction(100), 
        // Line 1: D=1-1;JGT (D=0. Jump if 0 > 0? NO)
        create_C_instruction(0b111010, 0b010, 0b001), 
        // Line 2: D=1;JEQ (D=1. Jump if 1 == 0? NO)
        create_C_instruction(0b111111, 0b010, 0b010),
        // Line 3: D=-1;JLT (D=-1. Jump if -1 < 0? YES)
        create_C_instruction(0b111010, 0b010, 0b100),
        // Line 4: D=0;JEQ (This line should be skipped by the jump)
        create_C_instruction(0b101010, 0b010, 0b010),
        // Line 5: JMP (Unconditional jump)
        create_C_instruction(0b101010, 0b000, 0b111) 
    };
    
    emu.loadProgram(program);
    
    // --- Step 1: Set Jump Target A=100 ---
    emu.executeNextInstruction(); // PC=0 -> PC=1. A=100
    REQUIRE(emu.getARegister() == 100);

    // --- Step 2: D=0;JGT (Test JGT) ---
    emu.executeNextInstruction(); // PC=1 -> PC=2. D=0. Jump NO (0 is not > 0)
    REQUIRE(emu.getDRegister() == 0);
    REQUIRE(emu.getPC() == 2);
    
    // --- Step 3: D=1;JEQ (Test JEQ) ---
    emu.executeNextInstruction(); // PC=2 -> PC=3. D=1. Jump NO (1 is not == 0)
    REQUIRE(emu.getDRegister() == 1);
    REQUIRE(emu.getPC() == 3);
    
    // --- Step 4: D=-1;JLT (Test JLT) ---
    emu.executeNextInstruction(); // PC=3 -> PC=100. D=-1. Jump YES (-1 is < 0). PC is set to A=100.
    REQUIRE(emu.getDRegister() == -1);
    REQUIRE(emu.getPC() == 100);
    
    // --- Step 5: Unconditional Jump (JMP) ---
    // Note: We need to reset PC and A to test the JMP instruction itself.
    emu.setPC(5);
    emu.setARegister(200);
    
    // Execute JMP instruction at Line 5: D=0;JMP (Result=0. All jump flags set to 1)
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 200);
}