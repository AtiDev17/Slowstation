cmake_minimum_required(VERSION 3.20)
project(SlowStation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(SlowStation ${SOURCES})
