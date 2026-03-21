#include <catch2/catch_test_macros.hpp>
#include "Emulators/VMEmulator/VMEmulator.hpp" 

TEST_CASE("VM Decoder: Arithmetic", "[decoder]") {
    VMEmulator vm;

    SECTION("Binary Operations") {
        std::vector<std::string> mathOps = {"add", "sub"};
        for (const auto& op : mathOps) {
            auto d = vm.decode(op);
            REQUIRE(d.type == InstructionType::BINARY_ARITHMETIC);
            REQUIRE(d.command == op);
        }

        std::vector<std::string> logicOps = {"and", "or"};
        for (const auto& op : logicOps) {
            auto d = vm.decode(op);
            REQUIRE(d.type == InstructionType::BINARY_ARITHMETIC);
            REQUIRE(d.command == op);
        }
        
        std::vector<std::string> compOps = {"eq", "gt", "lt"};
        for (const auto& op : compOps) {
            auto d = vm.decode(op);
            REQUIRE(d.type == InstructionType::BINARY_ARITHMETIC);
            REQUIRE(d.command == op);
        }
    }

    SECTION("Unary Operations") {
        std::vector<std::string> unaryOps = {"neg", "not"};
        for (const auto& op : unaryOps) {
            auto d = vm.decode(op);
            REQUIRE(d.type == InstructionType::UNARY_ARITHMETIC);
            REQUIRE(d.command == op);
        }
    }
}

TEST_CASE("VM Decoder: Push and Pop", "[decoder]") {
    VMEmulator vm;

    SECTION("Valid Push Commands") {
        auto d = vm.decode("push constant 42");
        REQUIRE(d.type == InstructionType::PUSH);
        REQUIRE(d.segment == Segment::CONSTANT);
        REQUIRE(d.value == 42);

        d = vm.decode("push local 0");
        REQUIRE(d.type == InstructionType::PUSH);
        REQUIRE(d.segment == Segment::LOCAL);
        REQUIRE(d.value == 0);
    }

    SECTION("Valid Pop Commands") {
        auto d = vm.decode("pop temp 7");
        REQUIRE(d.type == InstructionType::POP);
        REQUIRE(d.segment == Segment::TEMP);
        REQUIRE(d.value == 7);

        d = vm.decode("pop pointer 1");
        REQUIRE(d.type == InstructionType::POP);
        REQUIRE(d.segment == Segment::POINTER);
        REQUIRE(d.value == 1);
    }
}