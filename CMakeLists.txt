cmake_minimum_required(VERSION 3.16)

# Define the project name, version, and the language (C++)
project(Nand2TetrisCompiler VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17 for modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ASSEMBLER_SOURCES
    src/HackAssembler/AssemblyCommandParser.cpp
    src/HackAssembler/CodeTable.cpp
    src/HackAssembler/HackAssembler.cpp
    src/HackAssembler/SymbolTable.cpp
)

# Define all source files for the application
set(APP_SOURCES 
    main.cpp 
    src/parser.cpp
)

# Define implementation files (excluding main.cpp) for linking into libraries/tests
set(APP_IMPLEMENTATION_SOURCES 
    src/parser.cpp
)

# -----------------------------------------------------------------
# 1. Main Application Target
# -----------------------------------------------------------------
add_executable(
    main_app
    ${APP_SOURCES}
    ${ASSEMBLER_SOURCES}
)

target_include_directories(
    main_app 
    PRIVATE 
    include
)

# -----------------------------------------------------------------
# 2. Catch2 Test Target Setup
# -----------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0 
)
FetchContent_MakeAvailable(Catch2)

# Define your test executable target
add_executable(
    compiler_tests
    test/parser_test.cpp
    test/HackAssembler/CodeTable_test.cpp
    test/HackAssembler/SymbolTable_test.cpp
    test/HackAssembler/AssemblyCommandParser_test.cpp
    ${APP_IMPLEMENTATION_SOURCES}
    ${ASSEMBLER_SOURCES}
)

# Include directories for tests
target_include_directories(
    compiler_tests 
    PRIVATE 
    include
    lib
)

# Link tests to the Catch2 library, which provides the main() function for running tests
target_link_libraries(
    compiler_tests 
    PRIVATE Catch2::Catch2WithMain
)

add_test(
    NAME compiler_tests
    COMMAND compiler_tests
)

add_executable(
    integration_tests
    test/HackAssembler/integration/hackAssemblerIntegration.cpp 
    src/parser.cpp
    ${ASSEMBLER_SOURCES} 
)

target_include_directories(
    integration_tests 
    PRIVATE 
    include 
)

target_link_libraries(
    integration_tests 
    PRIVATE 
    Catch2::Catch2WithMain # This single target handles both headers and main() function
)

add_test(
    NAME integration_tests
    COMMAND integration_tests
)