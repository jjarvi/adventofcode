#include <string>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

namespace day03 {

static const int Y_MAX = 1024;
static const int X_MAX = 1024;

using Canvas = std::array<std::array<int, X_MAX>, Y_MAX>;


class Rect
{
public:
    int id;
    int x;
    int y;
    int w;
    int h;

    bool operator!=(const Rect& other) const
    {
        return id != other.id;
    }
};

static Rect parseRect(const std::string& str)
{
    Rect r;
    size_t bp = str.find('#');
    size_t ip = str.find('@');
    size_t xp = str.find(',');
    size_t yp = str.find(':');
    size_t wp = str.find('x');
    r.id = std::stoi(str.substr(bp + 1, ip));
    r.x  = std::stoi(str.substr(ip + 1, xp));
    r.y  = std::stoi(str.substr(xp + 1, yp));
    r.w  = std::stoi(str.substr(yp + 1, wp));
    r.h  = std::stoi(str.substr(wp + 1));
    return r;
}


static std::unique_ptr<Canvas> createCanvas(const std::vector<Rect>& rects)
{
    auto canvas = std::make_unique<Canvas>();
    for (auto& a : *canvas)
    {
        a.fill(0);
    }

    for (auto& r : rects)
    {
        for (int x = r.x; x < r.x + r.w; ++x)
        {
            for (int y = r.y; y < r.y + r.h; ++y)
            {
                (*canvas)[x][y]++;
            }
        }
    }
    return canvas;
}

static int calculateOverlap(const std::vector<Rect>& rects)
{
    auto canvas = createCanvas(rects);

    int overlap = 0;
    for (auto& a : *canvas)
    {
        for (auto v : a)
        {
            if (v > 1)
            {
                overlap++;
            }
        }
    }
    return overlap;
}

static bool isOverlapping(const Rect& a, const Rect& b)
{
    return (a.x + a.w > b.x) && (a.x < b.x + b.w) &&
        (a.y + a.h > b.y) && (a.y < b.y + b.h);
}

static int findFirstNotOverlapping(const std::vector<Rect>& rects)
{
    for (auto& a : rects)
    {
        bool overlaps = false;

        for (auto& b : rects)
        {
            if (a != b && isOverlapping(a, b))
            {
                overlaps = true;
            }
        }
        if (!overlaps)
        {
            return a.id;
        }
    }
    return -1;
}

TEST(Day03, parseRect)
{
    Rect r = parseRect("#1 @ 2,3: 4x5");
    EXPECT_EQ(1, r.id);
    EXPECT_EQ(2, r.x);
    EXPECT_EQ(3, r.y);
    EXPECT_EQ(4, r.w);
    EXPECT_EQ(5, r.h);
}

TEST(Day03, parseRect_largeNumbers)
{
    Rect r = parseRect("#1234 @ 234,3567: 45678x567890");
    EXPECT_EQ(1234,   r.id);
    EXPECT_EQ(234,    r.x);
    EXPECT_EQ(3567,   r.y);
    EXPECT_EQ(45678,  r.w);
    EXPECT_EQ(567890, r.h);
}

TEST(Day03, parseRect_noWs)
{
    Rect r = parseRect("#1@2,3:4x5");
    EXPECT_EQ(1, r.id);
    EXPECT_EQ(2, r.x);
    EXPECT_EQ(3, r.y);
    EXPECT_EQ(4, r.w);
    EXPECT_EQ(5, r.h);
}

TEST(Day03, parseRect_muchWs)
{
    Rect r = parseRect("    #   1   @    2  ,   3  :  4    x    5     ");
    EXPECT_EQ(1, r.id);
    EXPECT_EQ(2, r.x);
    EXPECT_EQ(3, r.y);
    EXPECT_EQ(4, r.w);
    EXPECT_EQ(5, r.h);
}

TEST(Day03, noOverlap)
{
    std::vector<Rect> rects {{1, 0, 0, 2, 2}, {2, 2, 2, 2, 2}};
    EXPECT_EQ(0, calculateOverlap(rects));
}

TEST(Day03, overlap_partial)
{
    std::vector<Rect> rects {{1, 0, 0, 2, 2}, {2, 1, 1, 2, 2}};
    EXPECT_EQ(1, calculateOverlap(rects));
}

TEST(Day03, overlap_whole)
{
    std::vector<Rect> rects {{1, 0, 0, 2, 2}, {2, 0, 0, 2, 2}};
    EXPECT_EQ(4, calculateOverlap(rects));
}

TEST(Day03, isOverlapping)
{
    EXPECT_FALSE(isOverlapping({1, 0, 0, 1, 1}, {2, 1, 0, 1, 1}));
    EXPECT_TRUE(isOverlapping({1, 0, 0, 1, 1}, {2, 0, 0, 1, 1}));
    EXPECT_FALSE(isOverlapping({1, 0, 0, 1, 1}, {2, 0, 1, 1, 1}));
    EXPECT_TRUE(isOverlapping({1, 0, 0, 2, 2}, {2, 1, 1, 2, 2}));
}

TEST(Day03, notOverlapping)
{
    std::vector<Rect> rects {{1, 0, 0, 2, 2}, {2, 1, 1, 2, 2}, {3, 4, 4, 2, 2}};
    EXPECT_EQ(3, findFirstNotOverlapping(rects));
}

TEST(Day03, solution)
{
    std::ifstream input("../day03_input.txt");
    std::string claim;
    std::vector<Rect> claims;

    while (std::getline(input, claim))
    {
        claims.push_back(parseRect(claim));
    }

    EXPECT_EQ(111266, calculateOverlap(claims));
    EXPECT_EQ(266, findFirstNotOverlapping(claims));
}

}
