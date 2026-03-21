    #ifndef VM_EMULATOR_HPP
#define VM_EMULATOR_HPP

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <algorithm>


enum class InstructionType {
    PUSH,
    POP,
    UNARY_ARITHMETIC,
    BINARY_ARITHMETIC
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
    std::unordered_map<std::string, std::function<void()>> unaryOps;
    std::unordered_map<std::string, std::function<void()>> binaryOps;
    std::unordered_map<std::string, Segment> segmentMap;

    std::string fetch();
    void execute(DecodedInstruction decoded);
    void executePush(DecodedInstruction decoded);
    void executePop(DecodedInstruction decoded);
    void executeUnaryArithmetic(DecodedInstruction decoded);
    void executeBinaryArithmetic(DecodedInstruction decoded);
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
    
    void loadProgram(const std::vector<std::string>& instructions);
    DecodedInstruction decode (std::string instruction);
    void executeNextInstruction();
    int16_t peek(uint16_t addr) const;
};

#endif