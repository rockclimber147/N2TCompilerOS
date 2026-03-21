#include <catch2/catch_test_macros.hpp>
#include "Emulators/VMEmulator/VMEmulator.hpp" 

TEST_CASE("VM Stack: Pushing Constants", "[stack][constants]") {
    VMEmulator vm;

    // Standard initial state: SP (RAM[0]) should be 256
    REQUIRE(vm.peek(vm.STACK_POINTER) == 256);

    SECTION("Push a single constant") {
        vm.loadProgram({"push constant 42"});
        vm.executeNextInstruction();

        // 1. The value 42 should be at the old SP (256)
        REQUIRE(vm.peekStack() == 42);
        REQUIRE(vm.peek(256) == 42);

        // 2. The Stack Pointer should now be 257
        REQUIRE(vm.peek(vm.STACK_POINTER) == 257);
    }

    SECTION("Push multiple constants") {
        vm.loadProgram({"push constant 10", "push constant 20", "push constant 30"});
        
        vm.executeNextInstruction(); // SP becomes 257
        vm.executeNextInstruction(); // SP becomes 258
        vm.executeNextInstruction(); // SP becomes 259

        REQUIRE(vm.peekStack() == 30);
        REQUIRE(vm.peek(256) == 10);
        REQUIRE(vm.peek(257) == 20);
        REQUIRE(vm.peek(258) == 30);
        REQUIRE(vm.peek(vm.STACK_POINTER) == 259);
    }
}

TEST_CASE("VM Segment Access: Base Pointers", "[segments][pointer-based]") {
    VMEmulator vm;

    SECTION("Local Segment: Push and Pop") {
        // Setup: Set LCL base address to 300
        vm.poke(vm.LCL_POINTER, 300);
        
        // Test: Pop 999 into local 2 (Internally RAM[302])
        vm.loadProgram({"push constant 999", "pop local 2"});
        vm.executeNextInstruction(); // push 999
        vm.executeNextInstruction(); // pop local 2
        
        // Use peekLocal to verify the abstracted segment
        REQUIRE(vm.peekLocal(2) == 999);
        // Verify the raw RAM to ensure the pointer math was correct
        REQUIRE(vm.peek(302) == 999);

        // Test: push from local 2 back to stack
        vm.loadProgram({"push local 2"});
        vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == 999);
    }

    SECTION("Argument Segment: Offset logic") {
        // Setup: Set ARG base address to 400
        vm.poke(vm.ARG_POINTER, 400);
        vm.pokeArgument(5, 123); // Use helper to setup memory
        
        vm.loadProgram({"push argument 5"});
        vm.executeNextInstruction();
        
        REQUIRE(vm.peekStack() == 123);
    }
}

TEST_CASE("VM Segment Access: Fixed Mapping", "[segments][fixed]") {
    VMEmulator vm;

    SECTION("Temp Segment (RAM 5-12)") {
        vm.loadProgram({"push constant 55", "pop temp 3"});
        vm.executeNextInstruction();
        vm.executeNextInstruction(); 
        
        // Use peekTemp instead of raw TEMP_POINTER math
        REQUIRE(vm.peekTemp(3) == 55);
        REQUIRE(vm.peek(8) == 55);
    }

    SECTION("Pointer Segment (RAM 3-4)") {
        vm.loadProgram({"push constant 3000", "pop pointer 0", 
                        "push constant 4000", "pop pointer 1"});
        
        // Using a loop for multiple instructions
        for(int i=0; i<4; ++i) vm.executeNextInstruction();
        
        REQUIRE(vm.peekPointer(0) == 3000); // Should be RAM[3] (THIS)
        REQUIRE(vm.peekPointer(1) == 4000); // Should be RAM[4] (THAT)
        REQUIRE(vm.peek(3) == 3000);
    }

    SECTION("Static Segment (RAM 16-255)") {
        vm.loadProgram({"push constant 88", "pop static 5"});
        vm.executeNextInstruction();
        vm.executeNextInstruction(); 
        
        REQUIRE(vm.peekStatic(5) == 88);
        REQUIRE(vm.peek(21) == 88);
    }
}