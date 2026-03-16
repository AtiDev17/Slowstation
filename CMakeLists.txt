cmake_minimum_required(VERSION 3.20)
project(SlowStation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add all include directories
include_directories(include)

# Explicitly list source files for transparency and build reliability
set(SOURCES
    src/bus/Bios.cpp
    src/bus/Interconnect.cpp
    src/cpu/Cpu.cpp
    src/logger/Logger.cpp
        src/main.cpp
        include/cpu/Instruction.hpp
        include/bus/Ram.hpp
        src/bus/Ram.cpp
)

add_executable(SlowStation ${SOURCES})
