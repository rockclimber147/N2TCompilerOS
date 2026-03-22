#include "Emulators/VMEmulator/VMEmulator.hpp"
#include "Emulators/VMEmulator/VMParser.hpp"
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

VMEmulator::VMEmulator() {
    ram.resize(32768, 0);
    ram[0] = 256;
    initDispatchTables();
    initSegmentMap();
}

void VMEmulator::loadRawProgram(const std::vector<std::string>& instructions) {
    rom.clear();
    rom = instructions;
    program_counter = 0;
}

void VMEmulator::loadProgram(const std::string& path) {
    parser.clear();

    if (fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".vm") {
                parser.loadFile(entry.path().string(), symbolTable);
            }
        }
    } else {
        parser.loadFile(path, symbolTable);
    }
    loadRawProgram(parser.getInstructions());
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
    } else if (firstWord == "goto") {
        decoded.type = InstructionType::GOTO;
        ss >> decoded.command;
    } else if (firstWord == "if-goto") {
        decoded.type = InstructionType::IF_GOTO;
        ss >> decoded.command;
    } else if (firstWord == "call") {
        decoded.type = InstructionType::FUNCTION_CALL;
        std::string funcName;
        int nArgs;
        
        if (ss >> funcName >> nArgs) {
            decoded.command = funcName;
            decoded.value = nArgs;
        } else {
            throw std::runtime_error("Invalid call command: " + instruction);
        }
    } else if (firstWord == "return") {
        decoded.type = InstructionType::RETURN;
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
        case InstructionType::FUNCTION_CALL:     executeFunctionCall(decoded);     break;
        case InstructionType::RETURN:            executeReturn(decoded);           break;
        case InstructionType::GOTO:
            program_counter = symbolTable.getAddressFromLabel(program_counter - 1, decoded.command);
            break;

        case InstructionType::IF_GOTO:
            if (stackPop() != 0) {
                program_counter = symbolTable.getAddressFromLabel(program_counter - 1, decoded.command);
            }
            break;
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

void VMEmulator::executeFunctionCall(DecodedInstruction decoded) {
    FunctionEntry entry = symbolTable.getFunctionAddress(decoded.command);
    stackPush(static_cast<int16_t>(program_counter));

    stackPush(ram[LCL_POINTER]);
    stackPush(ram[ARG_POINTER]);
    stackPush(ram[THIS_POINTER]);
    stackPush(ram[THAT_POINTER]);

    ram[ARG_POINTER] = ram[STACK_POINTER] - 5 - decoded.value;
    ram[LCL_POINTER] = ram[STACK_POINTER];

    for (int i = 0; i < entry.numLocals; ++i) {
        stackPush(0);
    }

    program_counter = entry.address;
}

void VMEmulator::executeReturn(DecodedInstruction decoded) {
    int16_t endFrame = ram[LCL_POINTER];
    int16_t retAddr = ram[endFrame - 5];
    ram[ram[ARG_POINTER]] = stackPop();
    ram[STACK_POINTER] = ram[ARG_POINTER] + 1;

    ram[THAT_POINTER] = ram[endFrame - 1];
    ram[THIS_POINTER] = ram[endFrame - 2];
    ram[ARG_POINTER]  = ram[endFrame - 3];
    ram[LCL_POINTER]  = ram[endFrame - 4];

    program_counter = static_cast<uint16_t>(retAddr);
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