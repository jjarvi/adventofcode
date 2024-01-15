cmake_minimum_required(VERSION 3.1)

project(adventofcode)
set(adventofcode_VERSION_MAJOR 0)
set(adventofcode_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Werror")

add_subdirectory(2018)
add_subdirectory(2023)
