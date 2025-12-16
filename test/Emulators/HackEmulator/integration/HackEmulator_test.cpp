#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Emulators/HackEmulator/FileLoader.hpp"
#include "Emulators/HackEmulator/HackEmulator.hpp"

TEST_CASE("Hack Emulator runs BasicTest Test Case", "[HackEmulator][BasicTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/BasicTest/BasicTest.hack");
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

TEST_CASE("Hack Emulator runs PointerTest Test Case", "[HackEmulator][PointerTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/PointerTest/PointerTest.hack");
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

TEST_CASE("Hack Emulator runs SimpleAdd Test Case", "[HackEmulator][SimpleAdd]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/SimpleAdd/SimpleAdd.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);

    for (int i = 0; i < 600; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(0) == 257);
    REQUIRE(emu.peek(256) == 15);
}

TEST_CASE("Hack Emulator runs StaticTest Test Case", "[HackEmulator][StaticTest]") {
    FileLoader loader;
    HackEmulator emu;
    
    std::vector<int16_t> commands = loader.loadFile("../test/Emulators/HackEmulator/integration/TestCases/StaticTest/StaticTest.hack");
    emu.loadProgram(commands);

    emu.setRamValue(0, 256);

    for (int i = 0; i < 200; i++) {
        emu.executeNextInstruction();
    }

    REQUIRE(emu.peek(256) == 1110);
}