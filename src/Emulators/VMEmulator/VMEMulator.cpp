#include "Emulators/VMEmulator/VMEmulator.hpp"
#include <sstream>

VMEmulator::VMEmulator() {
    ram.resize(32768, 0);
}

void VMEmulator::executeNextInstruction() {
    std::string raw = fetch();
    if (raw.empty()) return;
    
    DecodedInstruction decoded = decode(raw);
    execute(decoded); 
}

std::string VMEmulator::fetch() {
    if (program_counter >= rom.size()) {
        return "";
    }
    return rom[program_counter++];
}

DecodedInstruction VMEmulator::decode(std::string instruction) {
    DecodedInstruction decoded;
    std::stringstream ss(instruction);
    std::string command;
    ss >> command;

    if (command == "push") {
        decoded.type = InstructionType::PUSH;
        // logic to parse segment and value...
    } else if (command == "pop") {
        decoded.type = InstructionType::POP;
        // logic to parse segment and value...
    } else {
        decoded.type = InstructionType::ARITHMETIC;
        // logic to identify specific op (add, sub, etc.)
    }

    return decoded;
}

void VMEmulator::execute(DecodedInstruction decoded) {
    // This is the core logic hub
    // if type == PUSH: handle segments
    // if type == ARITHMETIC: use stackPop() and stackPush()
}

void VMEmulator::stackPush(uint16_t val) {
    uint16_t sp = ram[STACK_POINTER];
    ram[sp] = val;
    ram[STACK_POINTER]++;
}

uint16_t VMEmulator::stackPop() {
    ram[STACK_POINTER]--;
    uint16_t sp = ram[STACK_POINTER];
    return ram[sp];
}

int16_t VMEmulator::peek(uint16_t addr) const {
    if (addr >= ram.size()) return 0;
    return ram[addr];
}