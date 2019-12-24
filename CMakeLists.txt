cmake_minimum_required(VERSION 3.1)
project(adventofcpp2018)

set(adventofcpp2018_VERSION_MAJOR 0)
set(adventofcpp2018_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Werror")

file(GLOB sources "src/*.cpp")
add_executable(aoc2018 ${sources})

add_subdirectory(src/googletest)
include_directories(src/googletest/googletest/include/)
include_directories(src/googletest/googlemock/include/)
enable_testing()

target_link_libraries(aoc2018 gtest gtest_main)
