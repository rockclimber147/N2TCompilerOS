# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Set your project name and the required C++ standard (C++17 is modern)
project(Nand2TetrisCompiler VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# -----------------------------------------------------------------
# 1. Main Application Target
# -----------------------------------------------------------------
# Define all your application source files (including headers in 'include')
add_executable(
    main_app
    main.cpp
    src/parser.cpp
)

# Tell the compiler where your header files are
target_include_directories(main_app PRIVATE include)

# -----------------------------------------------------------------
# 2. Catch2 Test Target (The Magic Part)
# -----------------------------------------------------------------
# Use CMake's built-in feature to download Catch2 source during configuration
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    # Use a specific tag for stability (e.g., v3.4.0)
    GIT_TAG v3.4.0 
)
FetchContent_MakeAvailable(Catch2)

# Define your test executable target
add_executable(
    compiler_tests
    test/parser_test.cpp
    # Note: Catch2 v3 does NOT require a separate main.cpp file with CATCH_CONFIG_MAIN 
)

# Link your tests to Catch2 and your application's source files
target_link_libraries(
    compiler_tests 
    PRIVATE Catch2::Catch2WithMain  # Links Catch2 and automatically provides the main() function
    main_app_objects                # This is a trick to link test code to the app's source
)