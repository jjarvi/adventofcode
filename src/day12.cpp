#include <gtest/gtest.h>
#include <set>
#include <vector>
#include <numeric>

namespace day12 {

class Pots
{
public:
    Pots(const std::string& initial)
    {
        for (size_t i = 0; i < initial.size(); ++i)
        {
            if (initial[i] == PLANT_SYMBOL)
            {
                plants.insert(i);
            }
        }
    }

    void addRule(const std::string& rule)
    {
        rules.push_back({});
        auto& r = rules.back();
        assert(r.size() == rule.size());
        for (size_t i = 0; i < rule.size(); ++i)
        {
            r[i] = rule[i] == PLANT_SYMBOL;
        }
    }

    void advance()
    {
        std::set<int> nextGen;

        int min = *plants.begin();
        int max = *plants.rbegin();

        for (int i = min - 2; i < max + 2; ++i)
        {
            for (auto& rule : rules)
            {
                if (isMatching(rule, i))
                {
                    nextGen.insert(i);
                    break;
                }
            }
        }

        plants.swap(nextGen);
    }

    int getSum() const
    {
        return std::accumulate(plants.begin(), plants.end(), 0);
    }

private:
    static const int RULE_SIZE = 5;
    static const char PLANT_SYMBOL = '#';
    std::vector<std::array<bool, RULE_SIZE>> rules;
    std::set<int> plants;

    bool isMatching(const std::array<bool, RULE_SIZE>& rule, int i) const
    {
        int offset = -2;
        for (auto b : rule)
        {
            if (b != (plants.count(i + offset) > 0))
            {
                return false;
            }
            offset++;
        }
        return true;
    }
};


TEST(Day12, example)
{
    Pots pots("#..#.#..##......###...###");
    pots.addRule("...##");
    pots.addRule("..#..");
    pots.addRule(".#...");
    pots.addRule(".#.#.");
    pots.addRule(".#.##");
    pots.addRule(".##..");
    pots.addRule(".####");
    pots.addRule("#.#.#");
    pots.addRule("#.###");
    pots.addRule("##.#.");
    pots.addRule("##.##");
    pots.addRule("###..");
    pots.addRule("###.#");
    pots.addRule("####.");

    for (int round = 0; round < 20; ++round)
    {
        pots.advance();
    }

    EXPECT_EQ(325, pots.getSum());
}


TEST(Day12, solution)
{
    Pots pots("##.#############........##.##.####..#.#..#.##...###.##......#.#..#####....##..#####..#.#.##.#.##");
    pots.addRule("###.#");
    pots.addRule(".####");
    pots.addRule("##...");
    pots.addRule("##.##");
    pots.addRule(".#.##");
    pots.addRule("#.#..");
    pots.addRule("...##");
    pots.addRule("####.");
    pots.addRule("#.#.#");
    pots.addRule("##.#.");
    pots.addRule(".#...");
    pots.addRule("###..");
    pots.addRule("##..#");
    pots.addRule("#..#.");
    pots.addRule(".#..#");
    pots.addRule(".#.#.");

    // Part 1
    int round;
    for (round = 0; round < 20; ++round)
    {
        pots.advance();
    }
    EXPECT_EQ(4110, pots.getSum());

    // Part 2
    int prev = 0;
    int prevDelta = 0;
    int nSameDeltas = 0;
    while (true)
    {
        pots.advance();
        int sum = pots.getSum();
        int delta = sum - prev;
        if (delta == prevDelta)
        {
            nSameDeltas++;
            if (nSameDeltas > 5)
            {
                // pattern found, extrapolate result
                EXPECT_EQ(2650000000466, sum + ((50000000000 - round - 1) * delta));
                break;
            }
        }
        else
        {
            nSameDeltas = 0;
        }

        prev = sum;
        prevDelta = delta;
        round++; 
    }
}

}
