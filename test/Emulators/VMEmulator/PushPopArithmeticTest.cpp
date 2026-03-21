#include <catch2/catch_test_macros.hpp>
#include "Emulators/VMEmulator/VMEmulator.hpp" 

TEST_CASE("VM Stack Arithmetic", "[arithmetic]") {
    VMEmulator vm;
    
    SECTION("Simple Addition") {
        vm.loadProgram({"push constant 7", "push constant 8", "add"});
        vm.executeNextInstruction(); // push 7
        vm.executeNextInstruction(); // push 8
        vm.executeNextInstruction(); // add
        
        // Result should be at RAM[256], SP should be 257
        REQUIRE(vm.peek(vm.STACK_POINTER) == 257);
        REQUIRE(vm.peekStack() == 15);
    }

    SECTION("Comparison Operators") {
        vm.loadProgram({"push constant 5", "push constant 5", "eq", "push constant 10", "gt"});
        vm.executeNextInstruction(); // push 5
        vm.executeNextInstruction(); // push 5
        vm.executeNextInstruction(); // eq -> pushes -1 (true)
        
        REQUIRE(vm.peekStack() == -1); 
        
        vm.executeNextInstruction(); // push 10
        vm.executeNextInstruction(); // gt -> is -1 > 10? No.
        
        REQUIRE(vm.peekStack() == 0); // Result is false
    }
}