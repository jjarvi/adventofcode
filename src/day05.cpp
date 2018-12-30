#include <string>
#include <fstream>

#include <gtest/gtest.h>


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
    return std::numeric_limits<size_t>::max();
}

static std::string findPolymer(const std::string& str)
{
    std::string polymer = str;
    size_t begin = 0;
    size_t pos = 0;
    while((pos = findReactingUnits(polymer, begin)) != std::numeric_limits<size_t>::max())
    {
        begin = pos > 0 ? pos - 1 : pos;
        polymer.erase(pos, 2);
    }
    return polymer;
}

TEST(Day05, example)
{
    EXPECT_EQ("dabCBAcaDA", findPolymer("dabAcCaCBAcCcaDA"));
}

TEST(Day05, solution)
{
    std::string line;
    std::ifstream input("../day05_input.txt");
    std::getline(input, line);
    EXPECT_EQ(9900, findPolymer(line).size());
}
