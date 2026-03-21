#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Emulators/VMEmulator/VMEmulator.hpp"

TEST_CASE("VM Emulator runs Project7/MemoryAccess/BasicTest Test Case", "[HackEmulator][BasicTest]") {
    VMEmulator emu;
    emu.loadProgram("../test/Emulators/VMEmulator/integration/TestCases/Project7/MemoryAccess/BasicTest/BasicTest.vm");

    emu.poke(0, 256);
    emu.poke(1, 300);
    emu.poke(2, 400);
    emu.poke(3, 3000);
    emu.poke(4, 3010);

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

TEST_CASE("VM Emulator runs Project7/MemoryAccess/PointerTest Test Case", "[HackEmulator][PointerTest]") {
    VMEmulator emu;
    emu.loadProgram("../test/Emulators/VMEmulator/integration/TestCases/Project7/MemoryAccess/PointerTest/PointerTest.vm");

    emu.poke(0, 256);

    for (int i = 0; i < 450; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(256) == 6084);
    REQUIRE(emu.peek(3) == 3030);
    REQUIRE(emu.peek(4) == 3040);
    REQUIRE(emu.peek(3032) == 32);
    REQUIRE(emu.peek(3046) == 46);
}

TEST_CASE("VM Emulator runs Project7/MemoryAccess/StaticTest Test Case", "[HackEmulator][StaticTest]") {
    VMEmulator emu;
    emu.loadProgram("../test/Emulators/VMEmulator/integration/TestCases/Project7/MemoryAccess/StaticTest/StaticTest.vm");

    emu.poke(0, 256);

    for (int i = 0; i < 200; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(256) == 1110);
}

TEST_CASE("VM Emulator runs Project7/StackArithmetic/SimpleAdd Test Case", "[HackEmulator][SimpleAdd]") {
    VMEmulator emu;
    emu.loadProgram("../test/Emulators/VMEmulator/integration/TestCases/Project7/StackArithmetic/SimpleAdd/SimpleAdd.vm");

    emu.poke(0, 256);

    for (int i = 0; i < 600; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 257);
    REQUIRE(emu.peek(256) == 15);
}

TEST_CASE("VM Emulator runs Project7/StackArithmetic/StackTest Test Case", "[HackEmulator][StackTest]") {
    VMEmulator emu;
    emu.loadProgram("../test/Emulators/VMEmulator/integration/TestCases/Project7/StackArithmetic/StackTest/StackTest.vm");

    emu.poke(0, 256);

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