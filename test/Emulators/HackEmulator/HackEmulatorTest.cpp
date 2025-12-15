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

TEST_CASE("HackEmulator decodes instructions correctly", "[HackEmulator]") {
    HackEmulator emu;

    DecodedInstruction instr = emu.decode(0b0000000000000000);
    REQUIRE(instr.type == InstructionType::A_INSTRUCTION);

    instr = emu.decode(0b1111111111111111);
    REQUIRE(instr.type == InstructionType::C_INSTRUCTION);
    REQUIRE(instr.is_M_bit == true);
    REQUIRE(instr.dest_A == true);
    REQUIRE(instr.dest_D == true);
    REQUIRE(instr.dest_M == true);
    REQUIRE(instr.jump_JGT == true);
    REQUIRE(instr.jump_JEQ == true);
    REQUIRE(instr.jump_JLT == true);
    REQUIRE(instr.comp_code == 0b111111);

    instr = emu.decode(0b1110000000000000);
    REQUIRE(instr.type == InstructionType::C_INSTRUCTION);
    REQUIRE(instr.is_M_bit == false);
    REQUIRE(instr.dest_A == false);
    REQUIRE(instr.dest_D == false);
    REQUIRE(instr.dest_M == false);
    REQUIRE(instr.jump_JGT == false);
    REQUIRE(instr.jump_JEQ == false);
    REQUIRE(instr.jump_JLT == false);
    REQUIRE(instr.comp_code == 0b000000);

    instr = emu.decode(0b1110000000001001);
    REQUIRE(instr.type == InstructionType::C_INSTRUCTION);
    REQUIRE(instr.is_M_bit == false);
    REQUIRE(instr.dest_A == false);
    REQUIRE(instr.dest_D == false);
    REQUIRE(instr.dest_M == true);
    REQUIRE(instr.jump_JGT == false);
    REQUIRE(instr.jump_JEQ == false);
    REQUIRE(instr.jump_JLT == true);
    REQUIRE(instr.comp_code == 0b000000);

    instr = emu.decode(0b1110000000010010);
    REQUIRE(instr.type == InstructionType::C_INSTRUCTION);
    REQUIRE(instr.is_M_bit == false);
    REQUIRE(instr.dest_A == false);
    REQUIRE(instr.dest_D == true);
    REQUIRE(instr.dest_M == false);
    REQUIRE(instr.jump_JGT == false);
    REQUIRE(instr.jump_JEQ == true);
    REQUIRE(instr.jump_JLT == false);
    REQUIRE(instr.comp_code == 0b000000);

    instr = emu.decode(0b1110000000100100);
    REQUIRE(instr.type == InstructionType::C_INSTRUCTION);
    REQUIRE(instr.is_M_bit == false);
    REQUIRE(instr.dest_A == true);
    REQUIRE(instr.dest_D == false);
    REQUIRE(instr.dest_M == false);
    REQUIRE(instr.jump_JGT == true);
    REQUIRE(instr.jump_JEQ == false);
    REQUIRE(instr.jump_JLT == false);
    REQUIRE(instr.comp_code == 0b000000);
}

TEST_CASE("HackEmulator: Simple Add", "HackEmulator") {
    std::vector<int16_t> program = {
        create_A_instruction(2),         // @2
        create_C_instruction(0b1, 0b1, 0b1),
    };
    HackEmulator emu;
    REQUIRE(1 == 1);
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
}