#include "Emulators/VMEmulator/VMEmulator.hpp"
#include <sstream>

VMEmulator::VMEmulator() {
    ram.resize(32768, 0);
    ram[0] = 256;
    initDispatchTables();
    initSegmentMap();
}

void VMEmulator::loadProgram(const std::vector<std::string>& instructions) {
    rom.clear();
    rom = instructions;
    program_counter = 0;
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
    binaryOps["and"] = [this]() { stackPush(stackPop() & stackPop()); };
    binaryOps["or"]  = [this]() { stackPush(stackPop() | stackPop()); };
    
    binaryOps["sub"] = [this]() { 
        int16_t y = stackPop(); 
        int16_t x = stackPop(); 
        stackPush(x - y); 
    };

    unaryOps["neg"] = [this]() { stackPush(-stackPop()); };
    unaryOps["not"] = [this]() { stackPush(~stackPop()); };
}

void VMEmulator::initSegmentMap() {
    segmentMap["local"]    = Segment::LOCAL;
    segmentMap["argument"] = Segment::ARG;
    segmentMap["this"]     = Segment::THIS;
    segmentMap["that"]     = Segment::THAT;
    segmentMap["constant"] = Segment::CONSTANT;
    segmentMap["static"]   = Segment::STATIC;
    segmentMap["pointer"]  = Segment::POINTER;
    segmentMap["temp"]     = Segment::TEMP;
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
        if (segmentMap.find(segStr) != segmentMap.end()) {
            decoded.segment = segmentMap[segStr];
        } else {
            throw std::runtime_error("Unknown segment: " + segStr);
        }
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
    int16_t valueToPush = 0;

    switch (decoded.segment) {
        case Segment::CONSTANT: valueToPush = decoded.value; break;
        case Segment::LOCAL:    valueToPush = peekLocal(decoded.value); break;
        case Segment::ARG:      valueToPush = peekArgument(decoded.value); break;
        case Segment::THIS:     valueToPush = peekThis(decoded.value); break;
        case Segment::THAT:     valueToPush = peekThat(decoded.value); break;
        case Segment::POINTER:  valueToPush = peekPointer(decoded.value); break;
        case Segment::TEMP:     valueToPush = peekTemp(decoded.value); break;
        case Segment::STATIC:   valueToPush = peekStatic(decoded.value); break;

        default: throw std::runtime_error("Unknown segment for push");
    }

    stackPush(valueToPush);
}

void VMEmulator::executePop(DecodedInstruction decoded) {
    int16_t val = stackPop();

    switch (decoded.segment) {
        case Segment::CONSTANT: 
            throw std::runtime_error("Cannot pop into constant segment");
            
        case Segment::LOCAL:    pokeLocal(decoded.value, val); break;
        case Segment::ARG:      pokeArgument(decoded.value, val); break;
        case Segment::THIS:     pokeThis(decoded.value, val); break;
        case Segment::THAT:     pokeThat(decoded.value, val); break;
        case Segment::POINTER:  pokePointer(decoded.value, val); break;
        case Segment::TEMP:     pokeTemp(decoded.value, val); break; 
        case Segment::STATIC:   pokeStatic(decoded.value, val); break;

        default: throw std::runtime_error("Unknown segment for pop");
    }
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

int16_t VMEmulator::peekStack() {
    return ram[ram[STACK_POINTER] - 1];
}

void VMEmulator::poke(uint16_t addr, int16_t value) {
    ram[addr] = value;
}