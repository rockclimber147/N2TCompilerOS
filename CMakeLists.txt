cmake_minimum_required(VERSION 3.16)

# Define the project name, version, and the language (C++)
project(Nand2TetrisCompiler VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17 for modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EMULATOR_SOURCES
    src/Emulators/HackEmulator/FileLoader.cpp
    src/Emulators/HackEmulator/HackEmulator.cpp
)

set(TOKENIZER_SOURCES
    src/Tokenizer/TokenTypes.cpp
    src/Tokenizer/Tokenizer.cpp
    src/Tokenizer/Token.cpp
    src/Tokenizer/TokenValidator.cpp
)

set(ASSEMBLER_SOURCES
    src/HackAssembler/AssemblyCommandParser.cpp
    src/HackAssembler/CodeTable.cpp
    src/HackAssembler/HackAssembler.cpp
    src/HackAssembler/SymbolTable.cpp
    src/HackAssembler/ListingFileWriter.cpp
)

set(VMTRANSLATOR_SOURCES
    src/VMTranslator/VMCodeWriter.cpp
    src/VMTranslator/VMSpecifications.cpp
    src/VMTranslator/VMCommandParser.cpp
    src/VMTranslator/VMTranslator.cpp
)

set(JACKCOMPILER_SOURCES
    src/JackCompiler/JackSpec.cpp
    src/JackCompiler/JackParser.cpp
    src/JackCompiler/IR/HighLevelIR.cpp
)

# Define all source files for the application
set(APP_SOURCES 
    main.cpp 
    src/parser.cpp
    src/FullCompiler.cpp
)

# Define implementation files (excluding main.cpp) for linking into libraries/tests
set(APP_IMPLEMENTATION_SOURCES 
    src/parser.cpp
    src/FileParser.cpp
    src/FullCompiler.cpp
)

# -----------------------------------------------------------------
# 1. Main Application Target
# -----------------------------------------------------------------
add_executable(
    main_app
    ${APP_SOURCES}
    ${ASSEMBLER_SOURCES}
    ${VMTRANSLATOR_SOURCES}
    ${TOKENIZER_SOURCES}
    ${JACKCOMPILER_SOURCES}
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

# -----------------------------------------------------------------
# Assembler unit tests
# -----------------------------------------------------------------
add_executable(
    assembler_unit_tests
    test/parser_test.cpp
    test/HackAssembler/CodeTable_test.cpp
    test/HackAssembler/SymbolTable_test.cpp
    test/HackAssembler/AssemblyCommandParser_test.cpp
    ${APP_IMPLEMENTATION_SOURCES}
    ${ASSEMBLER_SOURCES}
)

# Include directories for tests
target_include_directories(
    assembler_unit_tests 
    PRIVATE 
    include
    lib
)

# Link tests to the Catch2 library, which provides the main() function for running tests
target_link_libraries(
    assembler_unit_tests 
    PRIVATE Catch2::Catch2WithMain
)

add_test(
    NAME assembler_unit_tests
    COMMAND assembler_unit_tests
)

# -----------------------------------------------------------------
# Assembler integration tests
# -----------------------------------------------------------------

add_executable(
    assembler_integration_tests
    test/HackAssembler/integration/hackAssemblerIntegration.cpp 
    src/parser.cpp
    ${ASSEMBLER_SOURCES} 
)

target_include_directories(
    assembler_integration_tests 
    PRIVATE 
    include 
)

target_link_libraries(
    assembler_integration_tests 
    PRIVATE 
    Catch2::Catch2WithMain # This single target handles both headers and main() function
)

add_test(
    NAME assembler_integration_tests
    COMMAND assembler_integration_tests
)

# -----------------------------------------------------------------
# VMTranslator unit tests
# -----------------------------------------------------------------

add_executable(
    vmTranslator_unit_tests
    test/VMTranslator/VMCommandParser_test.cpp 
    src/parser.cpp
    ${VMTRANSLATOR_SOURCES} 
)

target_include_directories(
    vmTranslator_unit_tests 
    PRIVATE 
    include 
)

target_link_libraries(
    vmTranslator_unit_tests 
    PRIVATE 
    Catch2::Catch2WithMain # This single target handles both headers and main() function
)

add_test(
    NAME vmTranslator_unit_tests
    COMMAND vmTranslator_unit_tests
)

# -----------------------------------------------------------------
# JackCompiler unit tests
# -----------------------------------------------------------------

add_executable(
    JackCompiler_unit_tests
    test/JackCompiler/Tokenizer_test.cpp 
    test/JackCompiler/JackParserClassVarDec_test.cpp
    test/JackCompiler/JackParserStatements_test.cpp
    test/JackCompiler/JackParserExpressionTerm_test.cpp
    ${TOKENIZER_SOURCES}
    ${JACKCOMPILER_SOURCES}
)

target_include_directories(
    JackCompiler_unit_tests 
    PRIVATE 
    include 
)

target_link_libraries(
    JackCompiler_unit_tests 
    PRIVATE 
    Catch2::Catch2WithMain # This single target handles both headers and main() function
)

add_test(
    NAME JackCompiler_unit_tests
    COMMAND JackCompiler_unit_tests
)

# -----------------------------------------------------------------
# Emulator unit tests
# -----------------------------------------------------------------

add_executable(
    HackEmulator_unit_tests
    test/Emulators/HackEmulator/FileLoaderTest.cpp 
    test/Emulators/HackEmulator/HackEmulatorTest.cpp
    ${EMULATOR_SOURCES} 
)

target_include_directories(
    HackEmulator_unit_tests 
    PRIVATE 
    include 
)

target_link_libraries(
    HackEmulator_unit_tests 
    PRIVATE 
    Catch2::Catch2WithMain
)

add_test(
    NAME HackEmulator_unit_tests
    COMMAND HackEmulator_unit_tests
)

# -----------------------------------------------------------------
# Emulator integration tests
# -----------------------------------------------------------------

add_executable(
    HackEmulator_integration_tests
    test/Emulators/HackEmulator/integration/HackEmulator_test.cpp
    ${EMULATOR_SOURCES} 
)

target_include_directories(
    HackEmulator_integration_tests 
    PRIVATE 
    include 
)

target_link_libraries(
    HackEmulator_integration_tests 
    PRIVATE 
    Catch2::Catch2WithMain
)

add_test(
    NAME HackEmulator_integration_tests
    COMMAND HackEmulator_integration_tests
)