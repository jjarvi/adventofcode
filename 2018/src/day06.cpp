#include <vector>
#include <limits>
#include <string>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

namespace aoc2018::day06 {

static constexpr int infinite = -1;
static constexpr int invalidIndex = -1;

struct Point
{
    int x;
    int y;
};


class Map
{
public:
    Map(std::vector<Point> coordinates) :
        coordinates(coordinates),
        dimensions(getMaxDimensions(coordinates))
    {
        calculateAreasAroundCoordinates();
    }

    const std::vector<int>& getAreasAroundCoordinates() const
    {
        return areasAroundCoordinates;
    }

    static int getDistance(const Point& a, const Point& b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    int getLargestFiniteAreaAroundOneCoordinate() const
    {
        int max = 0;
        for (auto a : areasAroundCoordinates)
        {
            if (a > max)
            {
                max = a;
            }
        }
        return max;
    }

    int getAreaWithMaxDistanceToEachCoordinate(int maxDistance) const
    {
        int areaSize = 0;

        for (int y = 0; y <= dimensions.y; ++y)
        {
            for (int x = 0; x <= dimensions.x; ++x)
            {
                int sum = 0;
                for (auto& p : coordinates)
                {
                    sum += getDistance({x, y}, p);
                }

                if (sum < maxDistance)
                {
                    areaSize++;
                }
            }
        }

        return areaSize;
    }


private:
    Point getMaxDimensions(const std::vector<Point>& coordinates) const
    {
        Point max = {0, 0};

        for (auto& p : coordinates)
        {
            if (max.x < p.x)
            {
                max.x = p.x;
            }
            if (max.y < p.y)
            {
                max.y = p.y;
            }
        }

        return max;
    }

    int findNearestCoordinateOfPoint(const Point& p)
    {
        int nearest = invalidIndex;
        int min = std::numeric_limits<int>::max();
        int second = min;
        int i = 0;

        for (auto& c : coordinates)
        {
            int distance = getDistance(p, c);
            if (distance <= min)
            {
                second = min;
                min = distance;
                nearest = i;
            }
            i++;
        }

        return min != second ? nearest : invalidIndex;
    }

    bool isAtBorder(const Point& p) const
    {
        return p.x == 0 || p.y == 0 || p.x == dimensions.x || p.y == dimensions.y;
    }

    void mapToArea(const Point& p)
    {
        int i = findNearestCoordinateOfPoint(p);

        if (i != invalidIndex)
        {
            if (isAtBorder(p))
            {
                areasAroundCoordinates[i] = infinite;
            }
            else if (areasAroundCoordinates[i] != infinite)
            {
                areasAroundCoordinates[i]++;
            }
        }
    }

    void calculateAreasAroundCoordinates()
    {
        areasAroundCoordinates.resize(coordinates.size());

        for (int y = 0; y <= dimensions.y; ++y)
        {
            for (int x = 0; x <= dimensions.x; ++x)
            {
                mapToArea({x, y});
            }
        }
    }

    std::vector<Point> coordinates;
    std::vector<int> areasAroundCoordinates;
    Point dimensions;
};

TEST(Day06, distance)
{
    EXPECT_EQ(0, Map::getDistance({0, 0}, {0, 0}));
    EXPECT_EQ(1, Map::getDistance({0, 0}, {0, 1}));
    EXPECT_EQ(1, Map::getDistance({0, 0}, {1, 0}));
    EXPECT_EQ(2, Map::getDistance({0, 0}, {1, 1}));
}

TEST(Day06, part1_example)
{
    std::vector<Point> coordinates = {
        {1, 1},
        {1, 6},
        {8, 3},
        {3, 4},
        {5, 5},
        {8, 9}
    };

    Map map(coordinates);
    auto& areas = map.getAreasAroundCoordinates();
    EXPECT_EQ(infinite, areas[0]);
    EXPECT_EQ(infinite, areas[1]);
    EXPECT_EQ(infinite, areas[2]);
    EXPECT_EQ(9, areas[3]);
    EXPECT_EQ(17, areas[4]);
    EXPECT_EQ(infinite, areas[5]);
}

TEST(Day06, solution)
{
    std::vector<Point> coordinates;
    std::ifstream input(puzzleInputs::getInputDirectory() / "day06_input.txt");
    EXPECT_TRUE(input.is_open());
    std::string line;

    while (std::getline(input, line))
    {
        std::stringstream ss(line);
        std::string x;
        std::string y;
        std::getline(ss, x, ',');
        std::getline(ss, y, ',');
        coordinates.push_back({std::stoi(x), std::stoi(y)});
    }

    Map map(coordinates);
    EXPECT_EQ(3687, map.getLargestFiniteAreaAroundOneCoordinate());
    EXPECT_EQ(40134, map.getAreaWithMaxDistanceToEachCoordinate(10000));
}

}
