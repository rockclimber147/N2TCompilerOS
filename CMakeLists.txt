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