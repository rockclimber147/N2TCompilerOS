#ifndef HACK_EMULATOR_HPP
#define HACK_EMULATOR_HPP

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>


enum class InstructionType {
    A_INSTRUCTION,     // 0xxxxxxxxxxxxxxx
    C_INSTRUCTION      // 111xxxxxxxxxxxxx
};

struct DecodedInstruction {
    InstructionType type;
    int16_t addressOrValue;

    bool is_M_bit;     // xxx0xxxxxxxxxxxx
    
    uint8_t comp_code; // xxxx000000xxxxxx

    bool dest_A;       // xxxxxxxxxx001xxx
    bool dest_D;       // xxxxxxxxxx010xxx
    bool dest_M;       // xxxxxxxxxx100xxx

    bool jump_JGT;     // xxxxxxxxxxxxx001
    bool jump_JEQ;     // xxxxxxxxxxxxx010
    bool jump_JLT;     // xxxxxxxxxxxxx100
};


class HackEmulator {
private:
    // --- Memory Map Constants (Hack Architecture) ---
    // These define the base address for the memory segments in RAM
    const static uint16_t RAM_BASE_ADDR    = 0;
    const static uint16_t STATIC_BASE_ADDR = 16;

    const static uint16_t STACK_POINTER  = 0;
    const static uint16_t LCL_POINTER    = 1;
    const static uint16_t ARG_POINTER    = 2;
    const static uint16_t THIS_POINTER   = 3;
    const static uint16_t THAT_POINTER   = 4;

    const static uint16_t R_13          = 13;
    const static uint16_t R_14          = 14;
    const static uint16_t R_15          = 15;
    
    // Total size of RAM including I/O (16K Data + 8K Screen + 1 Key)
    const static uint16_t MEMORY_SIZE      = 24577; 
    
    // --- CPU Core Registers (16-bit) ---
    int16_t a_register = 0; 
    int16_t d_register = 0; 
    uint16_t program_counter = 0; 

    // --- Memory Units ---
    std::vector<int16_t> rom;  // Instruction Memory
    std::vector<int16_t> ram;  // Data Memory (including pointers and I/O)

    // --- Private Helper to Check RAM Bounds ---
    void checkRamAddress(uint16_t addr) const;

    int16_t fetch();
    DecodedInstruction decode(int16_t instruction);
    void execute(DecodedInstruction instruction);
    int16_t alu(bool is_M_bit, uint8_t operation);

public:
    // --- Constructor & Initialization ---
    HackEmulator() : ram(MEMORY_SIZE, 0) {} // Initialize RAM to zeros

    void loadProgram(const std::vector<int16_t>& instructions) {
        rom = instructions;
    } 

    // --- Execution Core (To be implemented later) ---
    void executeNextInstruction(); 

    // --- Public Test/Debug Accessors ---
    
    // 1. Register Getters
    int16_t getARegister() const { return a_register; }
    int16_t getDRegister() const { return d_register; }
    uint16_t getPC() const { return program_counter; }

    // 2. Direct Memory Access (M = RAM[A])
    int16_t getM() const { 
        checkRamAddress(a_register);
        return ram[a_register]; 
    }
    
    // 3. Segment Accessors (The methods you need for testing)
    
    // These methods return the value at a specific offset from a segment base address.
    
    int16_t getStatic(uint16_t offset) const {
        uint16_t addr = STATIC_BASE_ADDR + offset;
        checkRamAddress(addr);
        return ram[addr];
    }

    int16_t getThis(uint16_t offset) const;
    
    int16_t getThat(uint16_t offset) const;

    void setRamValue(uint16_t addr, int16_t value);
};

#endif