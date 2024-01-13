#include <string>
#include <fstream>

#include <gtest/gtest.h>

namespace day05 {

static bool doesReact(char a, char b)
{
    return (islower(a) && (toupper(a) == b)) ||
        (isupper(a) && (tolower(a) == b));
}

static size_t findReactingUnits(const std::string& str, size_t begin)
{
    char prev = ' ';
    for (size_t i = begin; i < str.size(); ++i)
    {
        char c = str[i];
        if (i > 0 && doesReact(prev, c))
        {
            return i - 1;
        }
        prev = c;
    }
    return std::string::npos;
}

static std::string findPolymer(const std::string& str)
{
    std::string polymer = str;
    size_t begin = 0;
    size_t pos = 0;
    while((pos = findReactingUnits(polymer, begin)) != std::string::npos)
    {
        begin = pos > 0 ? pos - 1 : pos;
        polymer.erase(pos, 2);
    }
    return polymer;
}

static size_t findShortestPolymer(const std::string& str)
{
    std::vector<size_t> sizes;
    for (char c = 'a'; c <= 'z'; c++)
    {
        std::string candidate = str;
        candidate.erase(std::remove_if(candidate.begin(), candidate.end(),
            [&](char r) -> bool {
                return ((c == r) || (c == tolower(r)));
            }), candidate.end());
        sizes.push_back(findPolymer(candidate).size());
    }
    std::sort(sizes.begin(), sizes.end());
    return sizes.front();
}


TEST(Day05, example1)
{
    EXPECT_EQ("dabCBAcaDA", findPolymer("dabAcCaCBAcCcaDA"));
}

TEST(Day05, example2)
{
    EXPECT_EQ(4, findShortestPolymer("dabAcCaCBAcCcaDA"));
}

TEST(Day05, solution)
{
    std::string line;
    std::ifstream input("../day05_input.txt");
    std::getline(input, line);
    EXPECT_EQ(9900, findPolymer(line).size());
    EXPECT_EQ(4992, findShortestPolymer(line));
}

}
