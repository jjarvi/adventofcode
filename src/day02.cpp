#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <utility>


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

std::pair<std::string, std::string> findWordsWhichDifferByOneLetter(
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

std::string removeDifferentLetters(std::pair<std::string, std::string> words)
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

    std::pair<std::string, std::string> ids = findWordsWhichDifferByOneLetter(
        {"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"});
    assert("fghij" == ids.first);
    assert("fguij" == ids.second);

    assert("fgij" == removeDifferentLetters(ids));
    assert("aaccc" == removeDifferentLetters({"aabbccbbbc", "aaddccdddc", }));
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
    std::cout << "Part 2 solution: " << removeDifferentLetters(
        findWordsWhichDifferByOneLetter(ids)) << std::endl;

    return 0;
}
