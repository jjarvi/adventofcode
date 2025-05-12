#include <gtest/gtest.h>
#include <list>

namespace aoc2018::day09 {

static void incrementByTwo(std::list<int>::iterator& it, std::list<int>& container)
{
    it++;
    if (it == container.end())
    {
        it = container.begin();
    }
    it++;
}


static void decrement(std::list<int>::iterator& it, std::list<int>& container)
{
    if (it == container.begin())
    {
        it = container.end();
    }
    it--;
}


static long int getHighscore(int numOfPlayers, int rounds)
{
    std::list<int> marbles = {0};
    std::vector<long int> scores(numOfPlayers);
    int player = 0;
    auto current = marbles.begin();

    for (int i = 1; i <= rounds; ++i)
    {
        if (i % 23 != 0)
        {
            incrementByTwo(current, marbles);
            current = marbles.insert(current, i);
        }
        else
        {
            for (int j = 0; j < 7; ++j)
            {
                decrement(current, marbles);
            }
            auto pick = current;
            current++;
            scores[player] += i + *pick;
            marbles.erase(pick);
        }
        player = (player + 1) % numOfPlayers;
    }

    std::sort(scores.begin(), scores.end());
    return scores.back();
}


TEST(Day09, example)
{
    EXPECT_EQ(32, getHighscore(9, 25));
    EXPECT_EQ(8317, getHighscore(10, 1618));
    EXPECT_EQ(146373, getHighscore(13, 7999));
    EXPECT_EQ(2764, getHighscore(17, 1104));
    EXPECT_EQ(54718, getHighscore(21, 6111));
    EXPECT_EQ(37305, getHighscore(30, 5807));
}


TEST(Day09, solution)
{
    EXPECT_EQ(371284, getHighscore(473, 70904));
    EXPECT_EQ(3038972494, getHighscore(473, 70904 * 100));
}

}
