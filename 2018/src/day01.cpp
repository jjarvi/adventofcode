#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

namespace aoc2018::day01 {

static int sum(const std::vector<int>& values)
{
    int s = 0;

    for (auto& i : values)
    {
        s += i;
    }

    return s;
}

static int findFirstDuplicateFrequency(const std::vector<int>& values)
{
    std::set<int> frequencies;
    int freq = 0;

    while (true)
    {
        for (auto& d : values)
        {
            if (frequencies.count(freq) == 0)
            {
                frequencies.insert(freq);
            }
            else
            {
                return freq;
            }
            freq += d;
        }
    }
}

TEST(Day01, sum)
{
    EXPECT_EQ(3, sum({1, 1, 1}));
}

TEST(Day01, duplicates)
{
    EXPECT_EQ(2,  findFirstDuplicateFrequency({1, -2, 3, 1}));
    EXPECT_EQ(0,  findFirstDuplicateFrequency({1, -1}));
    EXPECT_EQ(10, findFirstDuplicateFrequency({3, 3, 4, -2, -4}));
    EXPECT_EQ(5,  findFirstDuplicateFrequency({-6, 3, 8, 5, -6}));
    EXPECT_EQ(14, findFirstDuplicateFrequency({7, 7, -2, -7, -4}));
}

TEST(Day01, solution)
{
    std::ifstream input(puzzleInputs::getInputDirectory() /  "day01_input.txt");
    EXPECT_TRUE(input.is_open());
    std::vector<int> values;
    std::string line;
    while (std::getline(input, line))
    {
        std::stringstream ss(line);
        std::string num;
        while (std::getline(ss, num, ' '))
        {
            values.push_back(std::stoi(num));
        }
    }

    EXPECT_EQ(420, sum(values));
    EXPECT_EQ(227, findFirstDuplicateFrequency(values));
}

}
