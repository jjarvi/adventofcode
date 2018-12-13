#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>


std::map<char, int> getLetterDistribution(std::string word)
{
    std::map<char, int> letters;
        
    for (auto& c : word)
    {
        if (letters.count(c) == 0)
        {
            letters[c] = 1;
        }
        else
        {
            letters[c]++;
        }
    }

    return letters;
}

bool containsLetterNTimes(const std::map<char, int>& distribution, int n)
{
    for (auto& c : distribution)
    {
        if (c.second == n)
        {
            return true;
        }
    }
    return false;
}

bool containsLetterTwice(const std::map<char, int>& distribution)
{
    return containsLetterNTimes(distribution, 2);
}

bool containsLetterThreeTimes(const std::map<char, int>& distribution)
{
    return containsLetterNTimes(distribution, 3);
}

int calculateChecksum(const std::vector<std::string>& ids)
{
    int letterTwiceCount = 0;
    int letterThreeTimesCount = 0;
    
    for (auto& id : ids)
    {
        std::map<char, int> distribution = getLetterDistribution(id);
        if (containsLetterTwice(distribution))
        {
            letterTwiceCount++;
        }
        if (containsLetterThreeTimes(distribution))
        {
            letterThreeTimesCount++;
        }
    }

    return letterTwiceCount * letterThreeTimesCount;
}

void test()
{
    assert(false == containsLetterTwice(getLetterDistribution("abcdef")));
    assert(false == containsLetterThreeTimes(getLetterDistribution("abcdef")));
    assert(true == containsLetterTwice(getLetterDistribution("bababc")));
    assert(true == containsLetterThreeTimes(getLetterDistribution("bababc")));
    assert(true == containsLetterTwice(getLetterDistribution("abbcde")));
    assert(false == containsLetterThreeTimes(getLetterDistribution("abbcde")));
    assert(false == containsLetterTwice(getLetterDistribution("abcccd")));
    assert(true == containsLetterThreeTimes(getLetterDistribution("abcccd")));
    assert(true == containsLetterTwice(getLetterDistribution("aabcdd")));
    assert(false == containsLetterThreeTimes(getLetterDistribution("aabcdd")));
    assert(true == containsLetterTwice(getLetterDistribution("abcdee")));
    assert(false == containsLetterThreeTimes(getLetterDistribution("abcdee")));
    assert(false == containsLetterTwice(getLetterDistribution("ababab")));
    assert(true == containsLetterThreeTimes(getLetterDistribution("ababab")));

    assert(12 == calculateChecksum(
        {"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"}));

}

int main(void)
{
    test();

    std::vector<std::string> ids;

    std::string id;
    while (std::getline(std::cin, id))
    {
        ids.push_back(id);
    }

    std::cout << "Part 1 solution: " << calculateChecksum(ids) << std::endl;

    return 0;
}
