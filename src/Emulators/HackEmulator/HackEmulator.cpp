#include "Emulators/HackEmulator/HackEmulator.hpp"
#include <stdexcept>
#include <iostream>

// --- Constructor & Initialization ---

HackEmulator::HackEmulator() : ram(MEMORY_SIZE, 0),
                                a_register(0), 
                                d_register(0), 
                                program_counter(0)
{

}

// --- Program Loading ---

void HackEmulator::loadProgram(const std::vector<int16_t>& instructions) {
    program_counter = 0;
    rom = instructions;
}

void HackEmulator::checkRamAddress(uint16_t addr) const {
    if (addr >= MEMORY_SIZE) {
        throw std::out_of_range("Illegal memory access at " + std::to_string(addr)); 
    }
}

// --- Execution Core ---

void HackEmulator::executeNextInstruction() {
    int16_t instruction = fetch();
    DecodedInstruction decoded = decode(instruction);
    execute(decoded);
}

int16_t HackEmulator::fetch() {
    return rom[program_counter];
}

DecodedInstruction HackEmulator::decode(int16_t instruction) {
    DecodedInstruction decoded = {};
    if ((instruction & 0x8000) == 0) {
        decoded.type = InstructionType::A_INSTRUCTION;
        decoded.addressOrValue = instruction & 0x7FFF; 
        
    } else {
        decoded.type = InstructionType::C_INSTRUCTION;
        decoded.jump_JLT = (instruction >> 0) & 0x1;
        decoded.jump_JEQ = (instruction >> 1) & 0x1;
        decoded.jump_JGT = (instruction >> 2) & 0x1;

        decoded.dest_M = (instruction >> 3) & 0x1;
        decoded.dest_D = (instruction >> 4) & 0x1;
        decoded.dest_A = (instruction >> 5) & 0x1;

        decoded.is_M_bit = (instruction >> 12) & 0x1;
        decoded.comp_code = (instruction >> 6) & 0x3F;
    }
    
    return decoded;
}

void HackEmulator::execute(DecodedInstruction instruction) {
    if (instruction.type == InstructionType::A_INSTRUCTION) {
        a_register = instruction.addressOrValue;
        program_counter++;
    } else {
        int16_t m_value = getM(); // Value at RAM[A]
        int16_t alu_input_A = instruction.is_M_bit ? m_value : a_register; 
        
        // 2. Compute (Requires a separate ALU function using decoded.comp_code)
        int16_t result = alu(instruction.is_M_bit, instruction.comp_code); 
        
        if (instruction.dest_D) { d_register = result; }
        if (instruction.dest_A) { a_register = result; }
        if (instruction.dest_M) { setRamValue(a_register, result); }
        
        // 4. Jump/PC Update (Based on jump flags and result condition)
        bool jump_condition_met = instruction.jump_JGT && result > 0;
        jump_condition_met  = jump_condition_met || (instruction.jump_JEQ && result == 0);
        jump_condition_met  = jump_condition_met || (instruction.jump_JLT && result < 0);
        if (jump_condition_met) { program_counter = a_register; }
        else { program_counter++; }
    }
}

int16_t HackEmulator::alu(bool is_M_bit, uint8_t comp_code) {
    int16_t d = d_register;
    int16_t a_input = is_M_bit ? getM() : a_register;
    int16_t result = 0;

    switch (comp_code) {
        case 0b101010: result = 0; break;
        case 0b111111: result = 1; break;
        case 0b111010: result = -1; break;
        case 0b001100: result = d; break;
        case 0b110000: result = a_input; break;
        case 0b001101: result = ~d; break;
        case 0b110001: result = ~a_input; break;
        case 0b001111: result = -d; break;
        case 0b110011: result = -a_input; break;
        case 0b011111: result = d + 1; break;
        case 0b110111: result = a_input + 1; break;
        case 0b001110: result = d - 1; break;
        case 0b110010: result = a_input - 1; break;
        case 0b000010: result = d + a_input; break;
        case 0b010011: result = d - a_input; break;
        case 0b000111: result = a_input - d; break;
        case 0b000000: result = d & a_input; break;
        case 0b010101: result = d | a_input; break;
        default:
            throw std::runtime_error("Invalid ALU comp code: " + std::to_string(comp_code));
    }
    
    return result;
}



int16_t HackEmulator::getM() const { 
    checkRamAddress(a_register);
    return ram[a_register];
}

int16_t HackEmulator::getStatic(uint16_t offset) const {
    uint16_t addr = STATIC_BASE_ADDR + offset;
    checkRamAddress(addr);
    return ram[addr];
}

int16_t HackEmulator::getThis(uint16_t offset) const {
    uint16_t base = ram[THIS_POINTER];
    uint16_t addr = base + offset;
    checkRamAddress(addr);
    return ram[addr];
}

int16_t HackEmulator::getThat(uint16_t offset) const {
    uint16_t base = ram[THAT_POINTER]; 
    uint16_t addr = base + offset;
    checkRamAddress(addr);
    return ram[addr];
}

void HackEmulator::setRamValue(uint16_t addr, int16_t value) {
    checkRamAddress(addr);
    ram[addr] = value;
}