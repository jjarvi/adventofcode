#include <gtest/gtest.h>
#include <vector>
#include <limits>
#include <array>


class Square
{
public:
    Square(int x = 0, int y = 0, int size = 0, int power = std::numeric_limits<int>::min()) :
        x(x), y(y), size(size), power(power)
    {
    }

    bool operator<(const Square& other) const
    {
        return power < other.power;
    }

    bool operator==(const Square& other) const
    {
        return size == other.size && x == other.x && y == other.y && power && other.power;
    }

    int x;
    int y;
    int size;
    int power;
};


class PowerGrid
{
public:
    // 300+1 is a trick to avoid if else when populating table
    static constexpr size_t size = 301;
    using Grid = std::array<std::array<int, size>, size>;

    PowerGrid(int sn) : sn(sn)
    {
        Grid levels;
        for (auto& l : levels)
        {
            l.fill(0);
        }

        populateSummedAreaTable(levels);
        findHighestPowers(levels);
    }
    
    Square getSquareWithHighestPowerBySize(int squareSize) const
    {
        for (auto& s : highestPowers)
        {
            if (s.size == squareSize)
            {
                return s;
            }
        }
        return Square();
    }

    Square getSquareWithHighestPower()
    {
        std::sort(highestPowers.begin(), highestPowers.end());
        return highestPowers.back();
    }

private:
    int calculatePowerLevel(int x, int y) const
    {
        int rackId = x + 10;
        return ((rackId * y + sn) * rackId % 1000 / 100 - 5);
    }

    void populateSummedAreaTable(Grid& levels) const
    {
        for (size_t x = 1; x < size; ++x)
        {
            for (size_t y = 1; y < size; ++y)
            {
                levels[x][y] = calculatePowerLevel(x, y)
                    + levels[x][y - 1]
                    + levels[x - 1][y]
                    - levels[x - 1][y - 1];
            }
        }
    }

    void findHighestPowers(Grid& levels)
    {
        highestPowers.reserve(size);
        for (size_t squareSize = 1; squareSize < size; ++squareSize)
        {
            highestPowers.push_back(findSquareWithHighestPowerOfSize(levels, squareSize));
        }
    }

    Square findSquareWithHighestPowerOfSize(const Grid& levels, size_t squareSize) const
    {
        Square square(0, 0, squareSize);

        for (size_t x = squareSize; x < size; ++x)
        {
            for (size_t y = squareSize; y < size; ++y)
            {
                int power = levels[x][y]
                    + levels[x - squareSize ][y - squareSize ]
                    - levels[x - squareSize ][y]
                    - levels[x][y - squareSize ];

                if (power > square.power)
                {
                    square.power = power;
                    square.x = x - squareSize + 1;
                    square.y = y - squareSize + 1;
                    
                }
            }
        }
        
        return square;
    }

private:
    std::vector<Square> highestPowers;
    int sn;
};


TEST(Day11, solution)
{
    constexpr int sn = 5719;
    PowerGrid grid(sn);
    EXPECT_TRUE((Square(21, 34, 3) == grid.getSquareWithHighestPowerBySize(3)));
    EXPECT_TRUE((Square(90, 244, 16) == grid.getSquareWithHighestPower()));
}
