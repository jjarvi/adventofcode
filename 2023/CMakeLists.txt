add_executable(aoc2023
    src/main.cpp
    src/day01.cpp
    src/day02.cpp)

target_link_libraries(aoc2023
    CppUTest
    libPuzzleInputs
)

target_compile_definitions(aoc2023 PUBLIC APP_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
