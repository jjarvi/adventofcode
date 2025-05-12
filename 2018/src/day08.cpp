#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

namespace aoc2018::day08 {

class Parser
{
public:
    Parser(const std::vector<int>& input) :
        input(input),
        index(0),
        sum(0)
    {
        rootValue = parse();
    }

    int parse()
    {
        int nChildren = input[index++];
        int nMetadata = input[index++];
        int value = 0;
        std::vector<int> childValues;

        if (nChildren > 0)
        {
            for (int i = 0; i < nChildren; ++i)
            {   
                childValues.push_back(parse());
            }
        }

        for (int i = 0; i < nMetadata; i++)
        {
            int m = input[index++];
            sum += m;
            value += getNodeValueIncrement(m, childValues);
        }

        return value;
    }

    int getNodeValueIncrement(int metadata, const std::vector<int>& childValues) const
    {
        if (childValues.size() == 0)
        {
            return metadata;
        }
        else if (metadata > 0 && metadata <= static_cast<int>(childValues.size()))
        {
            return childValues[metadata - 1];
        }
        return 0;
    }

    int getSumOfMetadata() const
    {
        return sum;
    }

    int getRootValue() const
    {
        return rootValue;
    }

private:
    const std::vector<int>& input;
    int index;
    int sum;
    int rootValue;
};


TEST(Day08, metadataOnly)
{
    Parser p({0, 2, 10, 11});
    EXPECT_EQ(21, p.getSumOfMetadata());
}

TEST(Day08, oneChild)
{
    Parser p({1, 2, 0, 1, 2, 10, 11});
    EXPECT_EQ(23, p.getSumOfMetadata());
}

TEST(Day08, twoChildren)
{
    Parser p({2, 2, 0, 1, 5, 0, 0, 10, 11});
    EXPECT_EQ(26, p.getSumOfMetadata());
}

TEST(Day08, example)
{
    Parser p({2, 3, 0, 3, 10, 11, 12, 1, 1, 0, 1, 99, 2, 1, 1, 2});
    EXPECT_EQ(138, p.getSumOfMetadata());
    EXPECT_EQ(66, p.getRootValue());
}

TEST(Day08, solution)
{
    std::vector<int> tree;
    std::ifstream input(puzzleInputs::getInputDirectory() / "day08_input.txt");
    EXPECT_TRUE(input.is_open());
    std::string line;
    while (std::getline(input, line))
    {
        std::string num;
        std::stringstream ss(line);
        while (std::getline(ss, num, ' '))
        {
            tree.push_back(std::stoi(num));
        }
    }

    Parser p(tree);
    EXPECT_EQ(48260, p.getSumOfMetadata());
    EXPECT_EQ(25981, p.getRootValue());
}

}
