cmake_minimum_required(VERSION 3.1)

project(aoc2023)
set(aoc2023_VERSION_MAJOR 0)
set(aoc2023_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Werror")

add_executable(aoc2023
    src/main.cpp
    src/day01.cpp
    src/day02.cpp)

target_link_libraries(aoc2023 CppUTest)
