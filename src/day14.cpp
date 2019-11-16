#include <gtest/gtest.h>

#include <vector>
#include <string>

namespace day14 {

class Recipes
{
public:
    Recipes(size_t n) :
        recipes({3, 7})
    {
        int xi = 0;
        int yi = 1;
        int x = recipes[xi];
        int y = recipes[yi];

        while (recipes.size() < n)
        {
            int result = x + y;
            if (result > 9)
            {
                recipes.push_back(result / 10);
                recipes.push_back(result % 10);
            }
            else
            {
                recipes.push_back(result);
            }

            xi = (xi + 1 + x) % recipes.size();
            yi = (yi + 1 + y) % recipes.size();
            x = recipes.at(xi);
            y = recipes.at(yi);
        }
    }

    std::string get(size_t from, size_t len = 10)
    {
        std::string scores;
        for (size_t i = 0; i < len; ++i)
        {
            scores += std::to_string(recipes.at(i + from));
        }
        return scores;
    }

    int find(const std::vector<int> pattern)
    {
        auto it = std::search(recipes.begin(), recipes.end(), pattern.begin(), pattern.end());
        if (it == recipes.end())
        {
            return -1;
        }
        else
        {
            return it - recipes.begin();
        }
    }

private:
    std::vector<int> recipes;
};


TEST(Day14, solution)
{
    Recipes recipes(20231900);
    
    // Examples
    EXPECT_EQ("0124515891", recipes.get(5));    
    EXPECT_EQ("5158916779", recipes.get(9));    
    EXPECT_EQ("9251071085", recipes.get(18));    
    EXPECT_EQ("5941429882", recipes.get(2018));    

    // Part 1
    EXPECT_EQ("9315164154", recipes.get(505961));    

    // Part 2
    EXPECT_EQ(20231866, recipes.find({5, 0, 5, 9, 6, 1}));
}

}
