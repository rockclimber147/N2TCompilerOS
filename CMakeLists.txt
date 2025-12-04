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

# Create the executable target
add_executable(
    main_app
    ${APP_SOURCES}
)

# Tell the compiler where to find your headers (like "parser.hpp")
target_include_directories(
    main_app 
    PRIVATE 
    include
)

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
    test/parser_test.cpp         # <--- NEW TEST FILE (Where you'll write tests)
    ${APP_SOURCES}               # <--- Link ALL application logic into the test executable
)

# Include directories for tests
target_include_directories(
    compiler_tests 
    PRIVATE 
    include                     # To find CommandParser.hpp
    lib                         # To find fakeit.hpp (if you use it later)
)

# Link tests to the Catch2 library, which provides the main() function for running tests
target_link_libraries(
    compiler_tests 
    PRIVATE Catch2::Catch2WithMain
)