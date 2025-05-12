#include <array>
#include <fstream>
#include <map>
#include <string>
#include <span>
#include <filesystem>

#include <CppUTest/TestHarness.h>

#include "PuzzleInputs.hpp"

namespace aoc2023::day01
{
namespace
{
constexpr std::array digits(std::to_array<const char*>(
    {"1", "2", "3", "4", "5", "6", "7", "8", "9"}));

constexpr std::array digitsAndWords(std::to_array<const char*>(
    {"1", "2", "3", "4", "5", "6", "7", "8", "9",
     "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"}));

const std::map<const char*, int> translations{
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {"5", 5},
    {"6", 6},
    {"7", 7},
    {"8", 8},
    {"9", 9},
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9}
};
}

TEST_GROUP(Day01)
{
    using ListOfStrings = std::span<const char* const>;

    int sumCalibrationValues(const std::filesystem::path& filepath, const ListOfStrings allowedNumbers)
    {
        std::ifstream input(filepath);
        CHECK_TRUE(input.is_open());

        int sum = 0;
        std::string line;
        while (std::getline(input, line))
        {
            sum += parseCalibrationValue(line, allowedNumbers);
        }

        return sum;
    }

    int parseCalibrationValue(const std::string& input, const ListOfStrings allowedNumbers)
    {
        std::map<int, int> positions = findSymbols(input, allowedNumbers);
        return positions.begin()->second * 10 + positions.rbegin()->second;
    }

    std::map<int, int> findSymbols(const std::string& input, const ListOfStrings symbols)
    {
        std::map<int, int> positions;

        for (const char* symbol : symbols)
        {
            size_t pos = input.find(symbol);
            while (pos != std::string::npos)
            {
                positions[pos] = translate(symbol);
                pos = input.find(symbol, pos + 1);
            }
        }

        return positions;
    }

    int translate(const char* number)
    {
        return translations.at(number);
    }
};

TEST(Day01, part1InputDecoding)
{
    CHECK_EQUAL(12, parseCalibrationValue("1abc2", digits));
    CHECK_EQUAL(38, parseCalibrationValue("pqr3stu8vwx", digits));
    CHECK_EQUAL(15, parseCalibrationValue("a1b2c3d4e5f", digits));
    CHECK_EQUAL(77, parseCalibrationValue("treb7uchet", digits));
    CHECK_EQUAL(11, parseCalibrationValue("121", digits));
}

TEST(Day01, part2InputDecoding)
{
    CHECK_EQUAL(29, parseCalibrationValue("two1nine", digitsAndWords));
    CHECK_EQUAL(83, parseCalibrationValue("eightwothree", digitsAndWords));
    CHECK_EQUAL(13, parseCalibrationValue("abcone2threexyz", digitsAndWords));
    CHECK_EQUAL(24, parseCalibrationValue("xtwone3four", digitsAndWords));
    CHECK_EQUAL(42, parseCalibrationValue("4nineeightseven2", digitsAndWords));
    CHECK_EQUAL(14, parseCalibrationValue("zoneight234", digitsAndWords));
    CHECK_EQUAL(76, parseCalibrationValue("7pqrstsixteen", digitsAndWords));
    CHECK_EQUAL(88, parseCalibrationValue("eight", digitsAndWords));
    CHECK_EQUAL(82, parseCalibrationValue("eighttwo", digitsAndWords));
    CHECK_EQUAL(82, parseCalibrationValue("eightwo", digitsAndWords));
    CHECK_EQUAL(28, parseCalibrationValue("2eightwo8", digitsAndWords));
    CHECK_EQUAL(22, parseCalibrationValue("2eightwo", digitsAndWords));
    CHECK_EQUAL(11, parseCalibrationValue("onetwone", digitsAndWords));
}

TEST(Day01, part1)
{
    CHECK_EQUAL(53974, sumCalibrationValues(puzzleInputs::getInputDirectory() / "day01_input.txt", digits));
}

TEST(Day01, part2)
{
    CHECK_EQUAL(52840, sumCalibrationValues(puzzleInputs::getInputDirectory() / "day01_input.txt", digitsAndWords));
}
}
