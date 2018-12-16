#include <iostream>
#include <string>
#include <sstream>
#include <istream>
#include <assert.h>
#include <vector>
#include <set>


std::vector<int> parseInput(std::istream& input)
{
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

    return values;
}

int sum(const std::vector<int>& values)
{
    int s = 0;
    
    for (auto& i : values)
    {
        s += i;
    }

    return s;
}

int findFirstDuplicateFrequency(const std::vector<int>& deltas)
{
    std::set<int> frequencies;
    int freq = 0;

    while (true)
    {
        for (auto& d : deltas)
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

void testPart1(int result, const std::string& input)
{
    std::stringstream ss(input);
    assert(result == sum(parseInput(ss)));
}

void testPart2(int result, const std::vector<int>& deltas)
{
    assert(result == findFirstDuplicateFrequency(deltas));
}

int main(void)
{
    testPart1(3, "1, 1, 1");
    testPart1(3, "1, 1\n1");
    testPart1(0, "1, 1, -2");
    testPart1(-6, "-1, -2, -3");

    testPart2(2, {1, -2, 3, 1});
    testPart2(0, {1, -1});
    testPart2(10, {3, 3, 4, -2, -4});
    testPart2(5, {-6, 3, 8, 5, -6});
    testPart2(14, {7, 7, -2, -7, -4});

    auto input = parseInput(std::cin);
    std::cout << "Part 1 solution: " << sum(input) << std::endl;
    std::cout << "Part 2 solution: " << findFirstDuplicateFrequency(input) << std::endl;

    return 0;
}
