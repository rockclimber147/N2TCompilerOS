#include "Emulators/VMEmulator/VMEmulator.hpp"
#include <sstream>

VMEmulator::VMEmulator() {
    ram.resize(32768, 0);
    initDispatchTables();
}

void VMEmulator::initDispatchTables() {
    auto addComp = [this](std::string name, std::function<bool(int16_t, int16_t)> comp) {
        binaryOps[name] = [this, comp]() {
            int16_t y = stackPop();
            int16_t x = stackPop();
            stackPush(comp(x, y) ? -1 : 0);
        };
    };

    addComp("eq", [](int16_t a, int16_t b) { return a == b; });
    addComp("gt", [](int16_t a, int16_t b) { return a > b; });
    addComp("lt", [](int16_t a, int16_t b) { return a < b; });

    binaryOps["add"] = [this]() { stackPush(stackPop() + stackPop()); }; 
    
    binaryOps["sub"] = [this]() { 
        int16_t y = stackPop(); 
        int16_t x = stackPop(); 
        stackPush(x - y); 
    };

    unaryOps["neg"] = [this]() { stackPush(-stackPop()); };
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
    std::string firstWord;
    ss >> firstWord;

    if (firstWord == "push" || firstWord == "pop") {
        decoded.type = (firstWord == "push") ? InstructionType::PUSH : InstructionType::POP;
        std::string segStr;
        ss >> segStr >> decoded.value;
        // logic to map segStr to Segment enum...
    } else {
        if (binaryOps.find(firstWord) != binaryOps.end()) {
            decoded.type = InstructionType::BINARY_ARITHMETIC;
            decoded.command = firstWord;
        } else if (unaryOps.find(firstWord) != unaryOps.end()) {
            decoded.type = InstructionType::UNARY_ARITHMETIC;
            decoded.command = firstWord;
        }
    }
    return decoded;
}

void VMEmulator::execute(DecodedInstruction decoded) {
    switch (decoded.type) {
        case InstructionType::PUSH:              executePush(decoded);             break;
        case InstructionType::POP:               executePop(decoded);              break;
        case InstructionType::UNARY_ARITHMETIC:  executeUnaryArithmetic(decoded);  break;
        case InstructionType::BINARY_ARITHMETIC: executeBinaryArithmetic(decoded); break;
    }
}

void VMEmulator::executePush(DecodedInstruction decoded) {
    // 1. Calculate source address based on segment + value
    // 2. Read value from RAM
    // 3. stackPush(val)
}

void VMEmulator::executePop(DecodedInstruction decoded) {
    // 1. val = stackPop()
    // 2. Calculate target address based on segment + value
    // 3. ram[target] = val
}

void VMEmulator::executeBinaryArithmetic(DecodedInstruction decoded) {
    binaryOps[decoded.command]();
}

void VMEmulator::executeUnaryArithmetic(DecodedInstruction decoded) {
    unaryOps[decoded.command]();  
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