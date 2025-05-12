#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

namespace day10 {

constexpr int height = 120;
constexpr int width = 180;

struct Point
{
    int x;
    int y;
    int vx;
    int vy;
};


static void advance(std::vector<Point>& points)
{
    for (auto& p : points)
    {
        p.x += p.vx;
        p.y += p.vy;
    }
}


static bool isOnArea(const Point & p, int x, int y, int w, int h)
{
    return p.x >= x && p.x < w && p.y >= y && p.y < h;
}


static bool findVerticalLine(std::vector<Point>& points, int minLen)
{
    static bool canvas[width][height];
    memset(canvas, 0, sizeof(canvas));

    for (auto& p : points)
    {
        if (isOnArea(p, 0, 0, width, height))
        {
            canvas[p.x][p.y] = true;
        }
    }

    for (int x = 0; x < width; ++x)
    {
        int len = 0;
        for (int y = 0; y < height; ++y)
        {
            len = canvas[x][y] ? len + 1 : 0;
            if (len >= minLen)
            {
                return true;
            }
        }
    }
    return false;
}


static void print(std::vector<Point>& points)
{
    std::vector<std::string> lines(height);
    for (auto& line : lines)
    {
        line = std::string(width, '.');
    }

    for (auto& p : points)
    {
        if (isOnArea(p, 0, 0, width, height))
        {
            lines[p.y][p.x] = '#';
        }
    }

    for (auto& l : lines)
    {
        std::cout << l << std::endl;
    }
}


static Point parse(const std::string& line)
{
    constexpr size_t xPos = 10;
    constexpr size_t yPos = 18;
    constexpr size_t len = 6;
    constexpr size_t xvPos = 36;
    constexpr size_t yvPos = 40;
    constexpr size_t vLen = 2;

    return {std::stoi(line.substr(xPos, len)),
            std::stoi(line.substr(yPos, len)),
            std::stoi(line.substr(xvPos, vLen)),
            std::stoi(line.substr(yvPos, vLen))};
}


TEST(Day10, solution)
{
    constexpr bool printResult = false;
    constexpr size_t expectedLineSize = 43;
    std::vector<Point> points;
    std::ifstream input(puzzleInputs::getInputDirectory() / "day10_input.txt");
    EXPECT_TRUE(input.is_open());
    std::string line;

    while (std::getline(input, line))
    {
        if (line.size() == expectedLineSize)
        {
            Point p = parse(line);
            points.push_back(p);
        }
    }

    int seconds;
    constexpr int minLen = 10;
    for (seconds = 1; seconds < 20000; ++seconds)
    {
        advance(points);
        if (findVerticalLine(points, minLen))
        {
            if (printResult)
            {
                print(points);
            }
            break;
        }
    }
    EXPECT_EQ(10476, seconds);
}

}
