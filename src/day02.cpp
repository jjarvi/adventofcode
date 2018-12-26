#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <utility>

#include <gtest/gtest.h>


static std::map<char, int> getLetterDistribution(std::string word)
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

static bool containsLetterNTimes(const std::map<char, int>& distribution, int n)
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

static bool containsLetterTwice(const std::map<char, int>& distribution)
{
    return containsLetterNTimes(distribution, 2);
}

static bool containsLetterThreeTimes(const std::map<char, int>& distribution)
{
    return containsLetterNTimes(distribution, 3);
}

static int calculateChecksum(const std::vector<std::string>& ids)
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

static std::pair<std::string, std::string> findWordsWhichDifferByOneLetter(
    const std::vector<std::string>& words)
{
    for (int listPos = 0; listPos < words.size(); ++listPos)
    {
        for (int candidatePos = listPos + 1; candidatePos < words.size(); ++candidatePos)
        {
            int matches = 0;
            const std::string& word = words[listPos];
            const std::string& candidate = words[candidatePos];

            assert(word.size() == candidate.size());
            for (int i = 0; i < word.size(); ++i)
            {
                if (word[i] != candidate[i])
                {
                    matches++;
                }
            }

            if (matches == 1)
            {
                return std::pair<std::string, std::string>(word, candidate);
            }
        }
    }
    return std::pair<std::string, std::string>();
}

static std::string removeDifferentLetters(std::pair<std::string, std::string> words)
{
    assert(words.first.size() == words.second.size());

    for (int i = 0; i < words.first.size(); ++i)
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
    std::pair<std::string, std::string> ids = findWordsWhichDifferByOneLetter(
        {"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"});
    ASSERT_EQ("fghij", ids.first);
    ASSERT_EQ("fguij", ids.second);
}

TEST(Day02, removeDifferentLetters)
{
    std::pair<std::string, std::string> ids = findWordsWhichDifferByOneLetter(
        {"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"});
    ASSERT_EQ("fgij", removeDifferentLetters(ids));
    ASSERT_EQ("aaccc", removeDifferentLetters({"aabbccbbbc", "aaddccdddc", }));
}

TEST(Day02, solution)
{
    std::ifstream input("../day02_input.txt");
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
