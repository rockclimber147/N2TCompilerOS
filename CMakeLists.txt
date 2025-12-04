cmake_minimum_required(VERSION 3.16)

# Define the project name, version, and the language (C++)
project(Nand2TetrisCompiler VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17 for modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define all source files for the application
set(APP_SOURCES 
    main.cpp 
    src/parser.cpp
    src/AssemblyFileParser.cpp
)

# Define implementation files (excluding main.cpp) for linking into libraries/tests
set(APP_IMPLEMENTATION_SOURCES 
    src/parser.cpp
    src/AssemblyFileParser.cpp
)

# -----------------------------------------------------------------
# 1. Main Application Target
# -----------------------------------------------------------------
add_executable(
    main_app
    ${APP_SOURCES}
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
    ${APP_IMPLEMENTATION_SOURCES} # <--- CRITICAL CHANGE: Only link implementation files
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