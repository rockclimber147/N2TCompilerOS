#include <catch2/catch_test_macros.hpp>
#include "Emulators/HackEmulator/FileLoader.hpp" // Required for FileLoader (if used in other tests)
#include "Emulators/HackEmulator/HackEmulator.hpp" // The class under test
#include <vector>
#include <cstdint>

int16_t to_hack_instruction(int input) {
    return static_cast<int16_t>(input);
}

std::vector<int16_t> singleLineInstruction(int16_t command) {
    return {to_hack_instruction(command)};
}

TEST_CASE("HackEmulator decodes instructions correctly", "[HackEmulator][Decode]") {
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
    REQUIRE(instr.jump_JGT == true);
    REQUIRE(instr.jump_JEQ == false);
    REQUIRE(instr.jump_JLT == false);
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
    REQUIRE(instr.jump_JGT == false);
    REQUIRE(instr.jump_JEQ == false);
    REQUIRE(instr.jump_JLT == true);
    REQUIRE(instr.comp_code == 0b000000);
}

TEST_CASE("HackEmulator executes single instructions correctly", "[HackEmulator][Execute]") {
    HackEmulator emu;

    emu.reset();
    emu.loadProgram(singleLineInstruction(0b0000000000000010)); // @2
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 2);
    REQUIRE(emu.getPC() == 1);

    emu.reset();
    emu.loadProgram(singleLineInstruction(0b1110111111111000)); // ADM = 1
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 1);
    REQUIRE(emu.getDRegister() == 1);
    REQUIRE(emu.getM() == 1);
    REQUIRE(emu.getPC() == 1);

    emu.reset();
    emu.loadProgram(singleLineInstruction(0b1110111111000000)); // 1
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 0);
    REQUIRE(emu.getDRegister() == 0);
    REQUIRE(emu.getM() == 0);
    REQUIRE(emu.getPC() == 1);
}

TEST_CASE("HackEmulator handles jump logic correctly", "[HackEmulator][Jump]") {
    HackEmulator emu;

    emu.reset();
    std::vector<int16_t> commands = {
        to_hack_instruction(0b000000000001000), // @8
        to_hack_instruction(0b1110000000000111) // 0;JMP
    };
    emu.loadProgram(commands);
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 8);
    REQUIRE(emu.getPC() == 1);
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 8);

    commands = {
        to_hack_instruction(0b000000000001000), // @8
        to_hack_instruction(0b1110000000000001) // 0;JGT
    };
    emu.loadProgram(commands);
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 8);
    REQUIRE(emu.getPC() == 1);
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 2);

    commands = {
        to_hack_instruction(0b000000000001000), // @8
        to_hack_instruction(0b1110000000000100) // 0;JLT
    };
    emu.loadProgram(commands);
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 8);
    REQUIRE(emu.getPC() == 1);
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 2);

    emu.reset();
    commands = {
        to_hack_instruction(0b0000000000001000), // @8
        to_hack_instruction(0b1110111111010000), // D=1
        to_hack_instruction(0b1110001100000001)  // D;JGT
    };
    emu.loadProgram(commands);
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 8);
    REQUIRE(emu.getPC() == 1);
    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 1);
    REQUIRE(emu.getPC() == 2);
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 8);

    emu.reset();
    commands = {
        to_hack_instruction(0b0000000000001000), // @8
        to_hack_instruction(0b1110111111010000), // D=1
        to_hack_instruction(0b1110001100000010)  // D;JEQ
    };
    emu.loadProgram(commands);
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 8);
    REQUIRE(emu.getPC() == 1);
    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 1);
    REQUIRE(emu.getPC() == 2);
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 3);
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

TEST_CASE("Hackemulator: Simple Add", "[HackEmulator][Add]") {
    HackEmulator emu;
    // Adds 2 and 3 into address 0
    std::vector<int16_t> commands = {
        to_hack_instruction(0b0000000000000010), // @2
        to_hack_instruction(0b1110110000010000), // D=A
        to_hack_instruction(0b0000000000000011), // @3
        to_hack_instruction(0b1110000010010000), // D=D+A
        to_hack_instruction(0b0000000000000000), // @0
        to_hack_instruction(0b1110001100001000)  // M=D
    };

    emu.loadProgram(commands);

    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 2);
    REQUIRE(emu.getPC() == 1);

    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 2);
    REQUIRE(emu.getPC() == 2);

    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 3);
    REQUIRE(emu.getPC() == 3);

    emu.executeNextInstruction();
    REQUIRE(emu.getDRegister() == 5);
    REQUIRE(emu.getPC() == 4);

    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 0);
    REQUIRE(emu.getPC() == 5);

    emu.executeNextInstruction();
    REQUIRE(emu.getM() == 5);
    REQUIRE(emu.peek(0) == 5);
    REQUIRE(emu.getPC() == 6);
}

TEST_CASE("HackEmulator: JNE Jump Taken (D != 0)", "[HackEmulator][LoopPinpoint]") {
    HackEmulator emu;
    
    emu.reset();
    emu.setDRegister(5); // Simulate D being popped as 5 (n>0)

    std::vector<int16_t> commands = {
        to_hack_instruction(0b0000000000001010), // @10 (Target address)
        to_hack_instruction(0b1110001100000101)  // D;JNE (0;JLT|JGT)
    };
    
    emu.loadProgram(commands);
    
    // PC=0: Execute @10
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 10);
    REQUIRE(emu.getPC() == 1);

    // PC=1: Execute D;JNE. Result is D=5 (not 0). Jump must be taken.
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 10); // PC should jump to A=10
}

TEST_CASE("HackEmulator: JNE Jump Not Taken (D == 0)", "[HackEmulator][LoopPinpoint]") {
    HackEmulator emu;
    
    emu.reset();
    emu.setDRegister(0);

    std::vector<int16_t> commands = {
        to_hack_instruction(0b0000000000001010), // @10 (Target address)
        to_hack_instruction(0b1110001100000101)  // D;JNE (0;JLT|JGT)
    };
    
    emu.loadProgram(commands);
    
    emu.executeNextInstruction();
    REQUIRE(emu.getARegister() == 10);
    REQUIRE(emu.getPC() == 1);
    emu.executeNextInstruction();
    REQUIRE(emu.getPC() == 2);
}