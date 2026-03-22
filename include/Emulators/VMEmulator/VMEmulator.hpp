    #ifndef VM_EMULATOR_HPP
#define VM_EMULATOR_HPP

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include "VMParser.hpp"
#include "SymbolTable.hpp"


enum class InstructionType {
    PUSH,
    POP,
    UNARY_ARITHMETIC,
    BINARY_ARITHMETIC,
    GOTO,
    IF_GOTO,
    FUNCTION_CALL,
    RETURN
};

enum class Segment {
    CONSTANT,
    LOCAL,
    ARG,
    POINTER,
    THIS,
    THAT,
    STATIC,
    TEMP
};

struct DecodedInstruction {
    InstructionType type;
    Segment segment;
    std::string command;
    uint16_t value;
};


class VMEmulator {
private:
    std::vector<int16_t> ram;
    std::vector<std::string> rom;
    uint16_t program_counter = 0;
    VMParser parser;
    SymbolTable symbolTable;
    std::unordered_map<std::string, std::function<void()>> unaryOps;
    std::unordered_map<std::string, std::function<void()>> binaryOps;
    std::unordered_map<std::string, Segment> segmentMap;

    std::string fetch();
    void execute(DecodedInstruction decoded);
    void executePush(DecodedInstruction decoded);
    void executePop(DecodedInstruction decoded);
    void executeUnaryArithmetic(DecodedInstruction decoded);
    void executeBinaryArithmetic(DecodedInstruction decoded);
    void executeFunctionCall(DecodedInstruction decoded);
    void executeReturn(DecodedInstruction decoded);
    void stackPush(uint16_t val);
    uint16_t stackPop();

    void initDispatchTables();
    void initSegmentMap();

public:
    const static uint16_t RAM_BASE_ADDR    = 0;
    const static uint16_t STATIC_BASE_ADDR = 16;

    const static uint16_t STACK_POINTER    = 0;
    const static uint16_t LCL_POINTER      = 1;
    const static uint16_t ARG_POINTER      = 2;
    const static uint16_t POINTER_POINTER  = 3;
    const static uint16_t THIS_POINTER     = 3;
    const static uint16_t THAT_POINTER     = 4;
    const static uint16_t TEMP_POINTER     = 5;

    VMEmulator();
    
    void loadRawProgram(const std::vector<std::string>& instructions);
    void loadProgram(const std::string& path);

    DecodedInstruction decode (std::string instruction);
    void executeNextInstruction();
    int16_t peek(uint16_t addr) const;
    int16_t peekStack();
    void poke(uint16_t addr, int16_t value);

    int16_t peekLocal(uint16_t i) const    { return ram[ram[LCL_POINTER] + i]; }
    void pokeLocal(uint16_t i, int16_t v)  { ram[ram[LCL_POINTER] + i] = v; }

    int16_t peekArgument(uint16_t i) const { return ram[ram[ARG_POINTER] + i]; }
    void pokeArgument(uint16_t i, int16_t v) { ram[ram[ARG_POINTER] + i] = v; }

    int16_t peekThis(uint16_t i) const     { return ram[ram[THIS_POINTER] + i]; }
    void pokeThis(uint16_t i, int16_t v)   { ram[ram[THIS_POINTER] + i] = v; }

    int16_t peekThat(uint16_t i) const     { return ram[ram[THAT_POINTER] + i]; }
    void pokeThat(uint16_t i, int16_t v)   { ram[ram[THAT_POINTER] + i] = v; }

    // Fixed-Mapping Segments
    int16_t peekTemp(uint16_t i) const     { return ram[TEMP_POINTER + i]; }
    void pokeTemp(uint16_t i, int16_t v)   { ram[TEMP_POINTER + i] = v; }

    int16_t peekPointer(uint16_t i) const  { return ram[POINTER_POINTER + i]; }
    void pokePointer(uint16_t i, int16_t v) { ram[POINTER_POINTER + i] = v; }

    int16_t peekStatic(uint16_t i) const   { return ram[STATIC_BASE_ADDR + i]; }
    void pokeStatic(uint16_t i, int16_t v) { ram[STATIC_BASE_ADDR + i] = v; }
};

#endif