#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <utility>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

using LetterDistribution = std::map<char, int>;
using StringPair = std::pair<std::string, std::string>;


namespace aoc2018::day02 {

static LetterDistribution getLetterDistribution(std::string word)
{
    LetterDistribution letters;

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

static bool containsLetterNTimes(const LetterDistribution& d, int n)
{
    for (auto& c : d)
    {
        if (c.second == n)
        {
            return true;
        }
    }
    return false;
}

static bool containsLetterTwice(const LetterDistribution& d)
{
    return containsLetterNTimes(d, 2);
}

static bool containsLetterThreeTimes(const LetterDistribution& d)
{
    return containsLetterNTimes(d, 3);
}

static int calculateChecksum(const std::vector<std::string>& ids)
{
    int twice = 0;
    int threeTimes = 0;

    for (auto& id : ids)
    {
        LetterDistribution d = getLetterDistribution(id);
        if (containsLetterTwice(d))
        {
            twice++;
        }
        if (containsLetterThreeTimes(d))
        {
            threeTimes++;
        }
    }

    return twice * threeTimes;
}

static bool differByOneLetter(const std::string& a, const std::string& b)
{
    int matches = 0;
    assert(a.size() == b.size());
    for (unsigned int i = 0; i < a.size(); ++i)
    {
        if (a[i] != b[i])
        {
            matches++;
        }
    }
    return matches == 1;
}

static StringPair findWordsWhichDifferByOneLetter(const std::vector<std::string>& words)
{
    for (unsigned int listPos = 0; listPos < words.size(); ++listPos)
    {
        for (unsigned int candidatePos = listPos + 1; candidatePos < words.size(); ++candidatePos)
        {
            const std::string& word = words[listPos];
            const std::string& candidate = words[candidatePos];
            if (differByOneLetter(word, candidate))
            {
                return StringPair(word, candidate);
            }
        }
    }
    return StringPair();
}

static std::string removeDifferentLetters(StringPair words)
{
    assert(words.first.size() == words.second.size());

    for (unsigned int i = 0; i < words.first.size(); ++i)
    {
        if (words.first[i] != words.second[i])
        {
            words.first.erase(i, 1);
            words.second.erase(i, 1);
            return removeDifferentLetters({words.first, words.second});
        }
    }

    return words.first;
}

TEST(Day02, containsLetterTwice)
{
    EXPECT_TRUE(containsLetterTwice(getLetterDistribution("bababc")));
    EXPECT_TRUE(containsLetterTwice(getLetterDistribution("abbcde")));
    EXPECT_TRUE(containsLetterTwice(getLetterDistribution("aabcdd")));
    EXPECT_TRUE(containsLetterTwice(getLetterDistribution("abcdee")));
    EXPECT_FALSE(containsLetterTwice(getLetterDistribution("abcdef")));
    EXPECT_FALSE(containsLetterTwice(getLetterDistribution("abcccd")));
    EXPECT_FALSE(containsLetterTwice(getLetterDistribution("ababab")));
}

TEST(Day02, containsLetterThreeTimes)
{
    EXPECT_TRUE(containsLetterThreeTimes(getLetterDistribution("bababc")));
    EXPECT_TRUE(containsLetterThreeTimes(getLetterDistribution("abcccd")));
    EXPECT_TRUE(containsLetterThreeTimes(getLetterDistribution("ababab")));
    EXPECT_FALSE(containsLetterThreeTimes(getLetterDistribution("abbcde")));
    EXPECT_FALSE(containsLetterThreeTimes(getLetterDistribution("abcdef")));
    EXPECT_FALSE(containsLetterThreeTimes(getLetterDistribution("aabcdd")));
    EXPECT_FALSE(containsLetterThreeTimes(getLetterDistribution("abcdee")));
}

TEST(Day02, checksum)
{
    EXPECT_EQ(12, calculateChecksum(
        {"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"}));
}

TEST(Day02, differByOneLetter)
{
    StringPair ids = findWordsWhichDifferByOneLetter(
        {"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"});
    ASSERT_EQ("fghij", ids.first);
    ASSERT_EQ("fguij", ids.second);
}

TEST(Day02, removeDifferentLetters)
{
    StringPair ids = findWordsWhichDifferByOneLetter(
        {"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"});
    ASSERT_EQ("fgij", removeDifferentLetters(ids));
    ASSERT_EQ("aaccc", removeDifferentLetters({"aabbccbbbc", "aaddccdddc", }));
}

TEST(Day02, solution)
{
    std::ifstream input(puzzleInputs::getInputDirectory() / "day02_input.txt");
    EXPECT_TRUE(input.is_open());
    std::vector<std::string> ids;
    std::string id;

    while (std::getline(input, id))
    {
        ids.push_back(id);
    }

    EXPECT_EQ(7657, calculateChecksum(ids));
    EXPECT_EQ("ivjhcadokeltwgsfsmqwrbnuy",
        removeDifferentLetters(findWordsWhichDifferByOneLetter(ids)));
}

}
