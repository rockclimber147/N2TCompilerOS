#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Emulators/HackEmulator/FileLoader.hpp"
#include "Emulators/HackEmulator/HackEmulator.hpp"

TEST_CASE("Hack Emulator runs Project7/MemoryAccess/BasicTest Test Case", "[HackEmulator][BasicTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project7/MemoryAccess/BasicTest/BasicTest.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);
    emu.setRamValue(1, 300);
    emu.setRamValue(2, 400);
    emu.setRamValue(3, 3000);
    emu.setRamValue(4, 3010);

    for (int i = 0; i < 600; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(256) == 472);
    REQUIRE(emu.peek(300) == 10);
    REQUIRE(emu.peek(401) == 21);
    REQUIRE(emu.peek(402) == 22);
    REQUIRE(emu.peek(3006) == 36);
    REQUIRE(emu.peek(3012) == 42);
    REQUIRE(emu.peek(3015) == 45);
    REQUIRE(emu.peek(11) == 510);
}

TEST_CASE("Hack Emulator runs Project7/MemoryAccess/PointerTest Test Case", "[HackEmulator][PointerTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project7/MemoryAccess/PointerTest/PointerTest.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);

    for (int i = 0; i < 450; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(256) == 6084);
    REQUIRE(emu.peek(3) == 3030);
    REQUIRE(emu.peek(4) == 3040);
    REQUIRE(emu.peek(3032) == 32);
    REQUIRE(emu.peek(3046) == 46);
}

TEST_CASE("Hack Emulator runs Project7/MemoryAccess/StaticTest Test Case", "[HackEmulator][StaticTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project7/MemoryAccess/StaticTest/StaticTest.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);

    for (int i = 0; i < 200; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(256) == 1110);
}

TEST_CASE("Hack Emulator runs Project7/StackArithmetic/SimpleAdd Test Case", "[HackEmulator][SimpleAdd]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project7/StackArithmetic/SimpleAdd/SimpleAdd.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);

    for (int i = 0; i < 600; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 257);
    REQUIRE(emu.peek(256) == 15);
}

TEST_CASE("Hack Emulator runs Project7/StackArithmetic/StackTest Test Case", "[HackEmulator][StackTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project7/StackArithmetic/StackTest/StackTest.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);

    for (int i = 0; i < 600; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 266);
    REQUIRE(emu.peek(256) == -1);
    REQUIRE(emu.peek(257) == 0);
    REQUIRE(emu.peek(258) == 0);
    REQUIRE(emu.peek(259) == 0);
    REQUIRE(emu.peek(260) == -1);
    REQUIRE(emu.peek(261) == 0);
    REQUIRE(emu.peek(262) == -1);
    REQUIRE(emu.peek(263) == 0);
    REQUIRE(emu.peek(264) == 0);
    REQUIRE(emu.peek(265) == -91);
    
}

TEST_CASE("Hack Emulator runs Project8/Function Calls/FibonacciElement Test Case", "[HackEmulator][FibonacciElement]") {
    FileLoader loader;
    HackEmulator emu;

    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project8/Function Calls/FibonacciElement/FibonacciElement.hack");
    emu.loadProgram(commands);

    for (int i = 0; i < 6000; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 262);
    REQUIRE(emu.peek(261) == 3);
}


TEST_CASE("Hack Emulator runs Project8/Function Calls/NestedCall Test Case", "[HackEmulator][NestedCall]") {
    FileLoader loader;
    HackEmulator emu;

    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project8/Function Calls/NestedCall/Sys.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 261); // SP
    emu.setRamValue(1, 261); // LCL
    emu.setRamValue(2, 256); // ARG
    emu.setRamValue(3, -3);  // THIS
    emu.setRamValue(4, -4);  // THAT

    emu.setRamValue(5, -1);
    emu.setRamValue(6, -1);

    emu.setRamValue(256, 1234);
    emu.setRamValue(257, -1);
    emu.setRamValue(258, -2);
    emu.setRamValue(259, -3);
    emu.setRamValue(260, -4);

    for (int i = 261; i <= 299; i++) {
        emu.setRamValue(i, -1);
    }

    for (int i = 0; i < 4000; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 261);
    REQUIRE(emu.peek(1) == 261);
    REQUIRE(emu.peek(2) == 256);
    REQUIRE(emu.peek(3) == 4000);
    REQUIRE(emu.peek(4) == 5000);
    REQUIRE(emu.peek(5) == 135);
    REQUIRE(emu.peek(6) == 246);
}

TEST_CASE("Hack Emulator runs Project8/Function Calls/SimpleFunction Test Case", "[HackEmulator][SimpleFunction]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project8/Function Calls/SimpleFunction/SimpleFunction.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 317);
    emu.setRamValue(1, 317);
    emu.setRamValue(2, 310);
    emu.setRamValue(3, 3000);
    emu.setRamValue(4, 4000);
    emu.setRamValue(310, 1234);
    emu.setRamValue(311, 37);
    emu.setRamValue(312, 1000);
    emu.setRamValue(313, 305);
    emu.setRamValue(314, 300);
    emu.setRamValue(315, 3010);
    emu.setRamValue(316, 4010);

    for (int i = 0; i < 300; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 311);
    REQUIRE(emu.peek(1) == 305);
    REQUIRE(emu.peek(2) == 300);
    REQUIRE(emu.peek(3) == 3010);
    REQUIRE(emu.peek(4) == 4010);
    REQUIRE(emu.peek(310) == 1196);
}

TEST_CASE("Hack Emulator runs Project8/Function Calls/StaticsTest Test Case", "[HackEmulator][StaticsTest]") {
    FileLoader loader;
    HackEmulator emu;

    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project8/Function Calls/StaticsTest/StaticsTest.hack");
    emu.loadProgram(commands);
    emu.setRamValue(0, 256);

    for (int i = 0; i < 2500; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 263);
    REQUIRE(emu.peek(261) == -2);
    REQUIRE(emu.peek(262) == 8);
}

TEST_CASE("Hack Emulator runs Project8/Program Flow/BasicLoop Test Case", "[HackEmulator][BasicLoop]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project8/Program Flow/BasicLoop/BasicLoop.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);
    emu.setRamValue(1, 300);
    emu.setRamValue(2, 400);
    emu.setRamValue(400, 3);

    for (int i = 0; i < 600; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 257);
    REQUIRE(emu.peek(256) == 6);
}

TEST_CASE("Hack Emulator runs Project8/Program Flow/FibonacciSeries Test Case", "[HackEmulator][FibonacciSeries]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/Project8/Program Flow/FibonacciSeries/FibonacciSeries.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);
    emu.setRamValue(1, 300);
    emu.setRamValue(2, 400);
    emu.setRamValue(400, 6);
    emu.setRamValue(401, 3000);

    for (int i = 0; i < 1100; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(3000) == 0);
    REQUIRE(emu.peek(3001) == 1);
    REQUIRE(emu.peek(3002) == 1);
    REQUIRE(emu.peek(3003) == 2);
    REQUIRE(emu.peek(3004) == 3);
    REQUIRE(emu.peek(3005) == 5);
}

