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

    // --- Constructor & Initialization ---
    HackEmulator();

    void loadProgram(const std::vector<int16_t>& instructions);
    // --- Execution Core (To be implemented later) ---
    void executeNextInstruction(); 

    // --- Public Test/Debug Accessors ---
    int16_t getARegister() const { return a_register; }
    void setARegister(int16_t value) { a_register = value; }

    int16_t getDRegister() const { return d_register; }
    void setDRegister(int16_t value) { d_register = value; }

    uint16_t getPC() const { return program_counter; }
    void setPC(uint16_t value) { program_counter = value; }

    int16_t getM() const;

    int16_t getSP() const { return ram[STACK_POINTER]; }
    int16_t getLCL() const { return ram[LCL_POINTER]; }
    int16_t getARG() const { return ram[ARG_POINTER]; }
    int16_t getTHISPtr() const { return ram[THIS_POINTER]; }
    int16_t getTHATPtr() const { return ram[THAT_POINTER]; }
    
    int16_t getR13() const { return ram[R_13]; }
    int16_t getR14() const { return ram[R_14]; }
    int16_t getR15() const { return ram[R_15]; }
    
    // 3. Segment Accessors (The methods you need for testing)
    
    // These methods return the value at a specific offset from a segment base address.
    
    int16_t getStatic(uint16_t offset) const;

    int16_t getThis(uint16_t offset) const;
    
    int16_t getThat(uint16_t offset) const;

    void setRamValue(uint16_t addr, int16_t value);
};

#endif