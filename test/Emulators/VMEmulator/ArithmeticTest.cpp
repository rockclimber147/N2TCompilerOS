#include <catch2/catch_test_macros.hpp>
#include "Emulators/VMEmulator/VMEmulator.hpp" 

TEST_CASE("VM Stack Arithmetic: Binary Operations", "[arithmetic][binary]") {
    VMEmulator vm;

    SECTION("Basic Math (add, sub)") {
        vm.loadProgram({"push constant 10", "push constant 3", "add"});
        vm.executeNextInstruction(); // push 10
        vm.executeNextInstruction(); // push 3
        vm.executeNextInstruction(); // add
        REQUIRE(vm.peekStack() == 13);

        vm.loadProgram({"push constant 10", "push constant 3", "sub"});
        vm.executeNextInstruction(); // push 10
        vm.executeNextInstruction(); // push 3
        vm.executeNextInstruction(); // sub (10 - 3)
        REQUIRE(vm.peekStack() == 7);
    }

    SECTION("Bitwise Logic (and, or)") {
        // 5 is 0101, 3 is 0011. 5 & 3 = 0001 (1). 5 | 3 = 0111 (7).
        vm.loadProgram({"push constant 5", "push constant 3", "and"});
        vm.executeNextInstruction(); vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == 1);

        vm.loadProgram({"push constant 5", "push constant 3", "or"});
        vm.executeNextInstruction(); vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == 7);
    }

    SECTION("Comparisons (eq, gt, lt)") {
        // Test Equality
        vm.loadProgram({"push constant 8", "push constant 8", "eq"});
        vm.executeNextInstruction(); vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == -1); // True

        // Test Greater Than (x > y)
        vm.loadProgram({"push constant 10", "push constant 5", "gt"});
        vm.executeNextInstruction(); vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == -1); // 10 > 5 is True

        // Test Less Than (x < y)
        vm.loadProgram({"push constant 10", "push constant 5", "lt"});
        vm.executeNextInstruction(); vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == 0); // 10 < 5 is False
    }
}

TEST_CASE("VM Stack Arithmetic: Unary Operations", "[arithmetic][unary]") {
    VMEmulator vm;

    SECTION("Negation (neg)") {
        vm.loadProgram({"push constant 15", "neg"});
        vm.executeNextInstruction(); // push 15
        vm.executeNextInstruction(); // neg
        REQUIRE(vm.peekStack() == -15);

        vm.loadProgram({"push constant -10", "neg"});
        vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == 10);
    }

    SECTION("Bitwise NOT (not)") {
        // not 0 (0000...) should be -1 (1111...)
        vm.loadProgram({"push constant 0", "not"});
        vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == -1);

        // not -1 (1111...) should be 0 (0000...)
        vm.loadProgram({"push constant -1", "not"});
        vm.executeNextInstruction(); vm.executeNextInstruction();
        REQUIRE(vm.peekStack() == 0);
    }
}