#include <gtest/gtest.h>

#include <vector>
#include <fstream>
#include <array>


enum class Direction : int
{
    Left = 0,
    Right,
    Up,
    Down,
    NumOf
};


enum class Turn : int
{
    GoLeft = 0,
    GoStraight,
    GoRight,
    NumOf
};


enum class Symbol : char
{
    TrackHorizontal = '-',
    TrackVertical = '|',
    CartLeft = '<',
    CartRight = '>',
    CartUp = '^',
    CartDown = 'v',
    CornerDown = '\\',
    CornerUp = '/',
    Intersection = '+',
    Invalid = ' '
};


template<typename T>
static constexpr int toInt(T value)
{
    return static_cast<int>(value);
}


template<typename T>
static constexpr int toChar(T value)
{
    return static_cast<char>(value);
}


class Cart
{
public:
    Cart(int x, int y, Direction direction) :
        x(x),
        y(y),
        direction(direction),
        isCrashed(false),
        intersectionDecision(0)
    {
    }

    Turn decideAtIntersection()
    {
        Turn decision = static_cast<Turn>(intersectionDecision);
        makeNextIntersectionDecision();
        return decision;
    }

    bool collidesWith(Cart& other) const
    {
        return x == other.x && y == other.y;
    }

    int x;
    int y;
    Direction direction;
    bool isCrashed;


private:
    int intersectionDecision;

    void makeNextIntersectionDecision()
    {
        intersectionDecision++;
        if (intersectionDecision == toInt(Turn::NumOf))
        {
            intersectionDecision = 0;
        }
    }
};


class Carts : public std::vector<Cart>
{
public:
    Carts()
    {
    }
};


class Rails
{
public:
    static constexpr size_t MAX_SIZE = 150;

    Rails()
    {
        clear();
    }

    std::array<char, MAX_SIZE>& operator[](size_t y)
    {
        return container[y];
    }

    void clear()
    {
        for (auto& array : container)
        {
            array.fill(toChar(Symbol::Invalid));
        }
    }

private:
    std::array<std::array<char, MAX_SIZE>, MAX_SIZE> container;
};


class Navigator
{
public:
    std::array<std::map<char, Direction>, toInt(Direction::NumOf)> directions;

    Navigator()
    {
        directions[toInt(Direction::Left)][toChar(Symbol::TrackHorizontal)]  = Direction::Left;
        directions[toInt(Direction::Left)][toChar(Symbol::CornerDown)]       = Direction::Up;
        directions[toInt(Direction::Left)][toChar(Symbol::CornerUp)]         = Direction::Down;

        directions[toInt(Direction::Right)][toChar(Symbol::TrackHorizontal)] = Direction::Right;
        directions[toInt(Direction::Right)][toChar(Symbol::CornerDown)]      = Direction::Down;
        directions[toInt(Direction::Right)][toChar(Symbol::CornerUp)]        = Direction::Up;

        directions[toInt(Direction::Up)][toChar(Symbol::TrackVertical)]      = Direction::Up;
        directions[toInt(Direction::Up)][toChar(Symbol::CornerDown)]         = Direction::Left;
        directions[toInt(Direction::Up)][toChar(Symbol::CornerUp)]           = Direction::Right;

        directions[toInt(Direction::Down)][toChar(Symbol::TrackVertical)]    = Direction::Down;
        directions[toInt(Direction::Down)][toChar(Symbol::CornerDown)]       = Direction::Right;
        directions[toInt(Direction::Down)][toChar(Symbol::CornerUp)]         = Direction::Left;
    }

    void tick(Carts& carts, Carts& crashed, Rails& rails)
    {
        for (auto it = carts.begin(); it != carts.end(); ++it)
        {
            Cart& cart = *it;
            if (!cart.isCrashed)
            {
                chooseDirection(cart, rails);
                move(cart);
                markCrashes(it, carts);
            }
        }
        storeCrashed(carts, crashed);
    }

private:
    void move(Cart& cart) const
    {
        switch(cart.direction)
        {
            case Direction::Left:
                cart.x--;
                break;

            case Direction::Right:
                cart.x++;
                break;

            case Direction::Up:
                cart.y--;
                break;

            case Direction::Down:
                cart.y++;
                break;

            default:
                break;
        }
    }

    void markCrashes(Carts::iterator it, Carts& carts)
    {
        for (auto ot = carts.begin(); ot != carts.end(); ++ot)
        {
            if (it != ot && !ot->isCrashed && it->collidesWith(*ot))
            {
                it->isCrashed = true;
                ot->isCrashed = true;
                return;
            }
        }
    }

    void chooseDirection(Cart& cart, Rails& rails)
    {
        char c = rails[cart.y][cart.x];
        if (c != toChar(Symbol::Intersection))
        {
            assert(directions[toInt(cart.direction)].count(c) == 1);
            cart.direction = directions[toInt(cart.direction)][c];
        }
        else
        {
            Turn decision = cart.decideAtIntersection();
            if (decision == Turn::GoLeft)
            {
                cart.direction = turnLeft(cart.direction);
            }
            else if (decision == Turn::GoRight)
            {
                cart.direction = turnRight(cart.direction);
            }
        }
    }

    void storeCrashed(Carts& carts, Carts& crashed)
    {
        for (auto it = carts.begin(); it != carts.end();)
        {
            if (it->isCrashed)
            {
                crashed.push_back(*it);
                it = carts.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    Direction turnLeft(Direction current) const
    {
        if (current == Direction::Right)
        {
            return Direction::Up;
        }
        else if (current == Direction::Down)
        {
            return Direction::Right;
        }
        else if (current == Direction::Up)
        {
            return Direction::Left;
        }
        return Direction::Down;
    }

    Direction turnRight(Direction current) const
    {
        if (current == Direction::Up)
        {
            return Direction::Right;
        }
        else if (current == Direction::Right)
        {
            return Direction::Down;
        }
        else if (current == Direction::Left)
        {
            return Direction::Up;
        }
        return Direction::Left;
    }
};


class MapReader
{
public:
    static void read(std::istream& input, Carts& carts, Rails& rails)
    {
        int x = 0;
        int y = 0;
        std::string line;
        while(std::getline(input, line))
        {
            assert(line.size() <= Rails::MAX_SIZE);
            x = 0;
            for (auto c : line)
            {
                if (isCart(c))
                {
                    Cart cart(x, y, getDirection(c));
                    rails[y][x] = getRail(cart);
                    carts.push_back(cart);
                }
                else
                {
                    rails[y][x] = c;
                }
                x++;
            }
            y++;
        }
    }

private:
    static char getRail(Cart cart)
    {
        switch(cart.direction)
        {
            case Direction::Left:
            case Direction::Right:
                return toChar(Symbol::TrackHorizontal);

            default:
                return toChar(Symbol::TrackVertical);
        }
    }

    static bool isCart(char c)
    {
        switch(c)
        {
            case toChar(Symbol::CartLeft):
            case toChar(Symbol::CartRight):
            case toChar(Symbol::CartUp):
            case toChar(Symbol::CartDown):
                return true;

            default:
                return false;
        }
    }

    static Direction getDirection(char c)
    {
        switch(c)
        {
            case toChar(Symbol::CartLeft):
                return Direction::Left;
            case toChar(Symbol::CartRight):
                return Direction::Right;
            case toChar(Symbol::CartUp):
                return Direction::Up;
            default:
                return Direction::Down;
        }
    }
};



class Day13 : public ::testing::Test
{
public:
    virtual void SetUp()
    {
    }

    virtual void SetUp(const std::string& input)
    {
        std::stringstream ss(input);
        SetUp(ss);
    }

    virtual void SetUp(std::istream& input)
    {
        MapReader::read(input, carts, rails);
    }

    virtual void TearDown()
    {
        rails.clear();
        carts.clear();
        crashed.clear();
    }

    Navigator navigator;
    Rails rails;
    Carts carts;
    Carts crashed;
};


TEST_F(Day13, navigateSquareToRight)
{
    SetUp("/>-\\\n"
          "|  |\n"
          "|  |\n"
          "\\--/\n");

    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '\'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(1, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(2, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '/'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '\'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(2, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(1, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '/'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
}


TEST_F(Day13, navigateSquareToLeft)
{
    SetUp("/<-\\\n"
          "|  |\n"
          "|  |\n"
          "\\--/\n");

    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '/'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(1, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(2, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '\'
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '/'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(2, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '|'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(1, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '\'
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
    navigator.tick(carts, crashed, rails); // -> '-'
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
}


TEST_F(Day13, intersectionsFromRight)
{
    SetUp("  |\n"
          " ++\n"
          " + \n"
          ">+ \n");

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(3, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(2, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
}


TEST_F(Day13, intersectionsFromLeft)
{
    SetUp(" +<\n"
          " + \n"
          "++ \n"
          "|  \n");

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(0, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(2, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(2, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(3, carts[0].y);
}

TEST_F(Day13, intersectionsFromDown)
{
    SetUp("v   \n"
          "+++-\n"
          "  +-\n");

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(2, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(2, carts[0].y);
}

TEST_F(Day13, intersectionsFromUp)
{
    SetUp("-+  \n"
          " +++\n"
          "   ^\n");

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(3, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(2, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(1, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(1, carts[0].x);
    EXPECT_EQ(0, carts[0].y);

    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(0, carts[0].x);
    EXPECT_EQ(0, carts[0].y);
}


TEST_F(Day13, example)
{
    SetUp("/->-\\         \n"
          "|   |  /----\\ \n"
          "| /-+--+-\\  | \n"
          "| | |  | v  |  \n"
          "\\-+-/  \\-+--/\n"
          "  \\------/    \n");

    while (crashed.size() == 0)
    {
        navigator.tick(carts, crashed, rails);
    }
    EXPECT_EQ(7, crashed[0].x);
    EXPECT_EQ(3, crashed[0].y);
}


TEST_F(Day13, collisions)
{
    SetUp("-><<---\n");
    navigator.tick(carts, crashed, rails);
    EXPECT_EQ(2, crashed.size());
    EXPECT_EQ(2, crashed[0].x);
    EXPECT_EQ(2, crashed[1].x);
}


TEST_F(Day13, solution)
{
    std::ifstream input("../day13_input.txt");
    SetUp(input);

    // Part 1
    while (crashed.size() == 0)
    {
        navigator.tick(carts, crashed, rails);
    }
    EXPECT_EQ(43, crashed[0].x);
    EXPECT_EQ(111, crashed[0].y);

    // Part 2
    while (carts.size() > 1)
    {
        navigator.tick(carts, crashed, rails);
    }
    EXPECT_EQ(44, carts[0].x);
    EXPECT_EQ(56, carts[0].y);
}
