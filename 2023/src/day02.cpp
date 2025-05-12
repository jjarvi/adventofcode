#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <filesystem>

#include <CppUTest/TestHarness.h>

#include "PuzzleInputs.hpp"

namespace Aoc2023::Day02
{
namespace
{
class Cubes
{
public:
    Cubes(uint32_t r = 0, uint32_t g = 0, uint32_t b = 0) :
        red(r), green(g), blue(b)
    {
    }

    bool operator<(const Cubes& other) const
    {
        return red < other.red || green < other.green || blue < other.blue;
    }

    Cubes& operator+=(const Cubes& other)
    {
        red += other.red;
        green += other.green;
        blue += other.blue;
        return *this;
    }

    uint32_t power() const
    {
        return red * green * blue;
    }

    uint32_t red{0};
    uint32_t green{0};
    uint32_t blue{0};
};

class Game
{
public:
    Game(uint32_t idIn, const std::vector<Cubes>& turnsIn) : id(idIn), turns(turnsIn)
    {
    }

    uint32_t getId() const { return id; }

    const std::vector<Cubes>& getTurns() const { return turns; }

    bool isPlayableWith(const Cubes& availableCubes) const
    {
        for (const Cubes& cubesRequiredForTurn : turns)
        {
            if (availableCubes < cubesRequiredForTurn)
            {
                return false;
            }
        }
        return true;
    }

    Cubes getMinimumSetOfCubes() const
    {
        Cubes minimumSet;

        for (const Cubes& turn : turns)
        {
            minimumSet.red = std::max(minimumSet.red, turn.red);
            minimumSet.green = std::max(minimumSet.green, turn.green);
            minimumSet.blue = std::max(minimumSet.blue, turn.blue);
        }

        return minimumSet;
    }

private:
    uint32_t id;
    std::vector<Cubes> turns;
};

template<typename T>
T get(std::istringstream& input, const char* errorMessage)
{
    T value;
    if (input >> value)
    {
        return value;
    }
    throw std::runtime_error(errorMessage);
}


Cubes parseCubeType(const std::string& input)
{
    std::istringstream type(input);
    uint32_t num = get<uint32_t>(type, "Failed to determine number of cubes");
    std::string color = get<std::string>(type, "Failed to determine color of the cube");

    if (color == "red")
    {
        return Cubes(num, 0, 0);
    }
    else if (color == "green")
    {
        return Cubes(0, num, 0);
    }
    else if (color == "blue")
    {
        return Cubes(0, 0, num);
    }
    else
    {
        throw std::runtime_error("Unknown color: " + color);
    }
}

Cubes parseTurn(const std::string& definition)
{
    constexpr char delimiter = ',';
    std::istringstream turn(definition);
    Cubes cubes;
    std::string type;
    while (std::getline(turn, type, delimiter))
    {
        cubes += parseCubeType(type);
    }
    return cubes;
}

std::string extractTitle(std::istringstream& input)
{
    return get<std::string>(input, "Invalid game definition");
}

void validateTitle(const std::string& title)
{
    const std::string expectedTitle = "Game";
    if (title != expectedTitle)
    {
        throw std::runtime_error("Unexpected game title \"" + title + "\"");
    }
}

uint32_t extractId(std::istringstream& input)
{
    return get<uint32_t>(input, "Invalid or missing game identifier");
}

char extractEndMarker(std::istringstream& input)
{
    return get<char>(input, "Expected : after game identifier");
}

void validateEndMarker(char marker)
{
    constexpr char expectedEndMarker = ':';
    if (marker != expectedEndMarker)
    {
        throw std::runtime_error("Expected " + std::to_string(expectedEndMarker) +
            " after game identifier but got " + std::to_string(marker));
    }
}

uint32_t parseHeader(std::istringstream& input)
{
    validateTitle(extractTitle(input));
    uint32_t id = extractId(input);
    validateEndMarker(extractEndMarker(input));
    return id;
}

std::vector<Cubes> parseTurns(std::istringstream& input)
{
    std::vector<Cubes> turns;
    constexpr char turnDelimiter = ';';
    std::string turn;
    while (std::getline(input, turn, turnDelimiter))
    {
        turns.push_back(parseTurn(turn));
    }

    return turns;
}

Game makeGame(const std::string& definition)
{
    std::istringstream input(definition);
    uint32_t id = parseHeader(input);
    std::vector<Cubes> turns = parseTurns(input);
    return Game(id, turns);
}
}

TEST_GROUP(Utils)
{
};

TEST(Utils, cubeSetLessThan)
{
    CHECK_FALSE(Cubes(0, 0, 0) < Cubes(0, 0, 0));
    CHECK_TRUE(Cubes(0, 0, 0) < Cubes(1, 0, 0));
    CHECK_TRUE(Cubes(0, 0, 0) < Cubes(0, 1, 0));
    CHECK_TRUE(Cubes(0, 0, 0) < Cubes(0, 0, 1));
    CHECK_FALSE(Cubes(1, 0, 0) < Cubes(0, 0, 0));
}

TEST(Utils, invalidInput)
{
    CHECK_THROWS(std::runtime_error, makeGame(""));
    CHECK_THROWS(std::runtime_error, makeGame("Fame"));
    CHECK_THROWS(std::runtime_error, makeGame("Game a"));
}

TEST(Utils, id)
{
    CHECK_EQUAL(1, makeGame("Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green").getId());
    CHECK_EQUAL(123, makeGame("Game 123: 1 red").getId());
}

TEST(Utils, turns)
{
    Game game = makeGame("Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 20 green");

    CHECK_EQUAL(3, game.getTurns().size());

    CHECK_EQUAL(4, game.getTurns().at(0).red);
    CHECK_EQUAL(0, game.getTurns().at(0).green);
    CHECK_EQUAL(3, game.getTurns().at(0).blue);

    CHECK_EQUAL(1, game.getTurns().at(1).red);
    CHECK_EQUAL(2, game.getTurns().at(1).green);
    CHECK_EQUAL(6, game.getTurns().at(1).blue);

    CHECK_EQUAL(0, game.getTurns().at(2).red);
    CHECK_EQUAL(20, game.getTurns().at(2).green);
    CHECK_EQUAL(0, game.getTurns().at(2).blue);
}

TEST(Utils, singleTurn)
{
    Game game = makeGame("Game 1: 1 red");
    CHECK_EQUAL(1, game.getTurns().size());
    CHECK_EQUAL(1, game.getTurns().at(0).red);
    CHECK_EQUAL(0, game.getTurns().at(0).green);
    CHECK_EQUAL(0, game.getTurns().at(0).blue);
}

TEST(Utils, noTurns)
{
    Game game = makeGame("Game 1:");
    CHECK_EQUAL(0, game.getTurns().size());
}

TEST(Utils, isPlayableWith)
{
    Cubes availableCubes(12, 13, 14);

    CHECK_TRUE(makeGame("Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green")
        .isPlayableWith(availableCubes));
    CHECK_TRUE(makeGame("Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue")
        .isPlayableWith(availableCubes));
    CHECK_FALSE(makeGame("Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red")
        .isPlayableWith(availableCubes));
    CHECK_FALSE(makeGame("Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red")
        .isPlayableWith(availableCubes));
    CHECK_TRUE(makeGame("Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green")
        .isPlayableWith(availableCubes));
}

TEST(Utils, minimumSetOfCubes)
{
    Cubes expectedSet(3, 4, 5);
    Game game = makeGame("Game 1: 1 blue, 2 green; 3 red, 1 blue; 1 red, 4 green, 5 blue");
    Cubes actualSet = game.getMinimumSetOfCubes();
    CHECK_EQUAL(expectedSet.red, actualSet.red);
    CHECK_EQUAL(expectedSet.green, actualSet.green);
    CHECK_EQUAL(expectedSet.blue, actualSet.blue);
}

TEST(Utils, powerOfCubeSet)
{
    Cubes cubes(4, 2, 6);
    CHECK_EQUAL(48, cubes.power());
}

TEST_GROUP(Solution)
{
    void setup()
    {
        input.open(puzzleInputs::getInputDirectory() / "day02_input.txt");
        CHECK_TRUE(input.is_open());
    }

    std::ifstream input;
};

TEST(Solution, part1)
{
    uint32_t sumOfIds = 0;
    Cubes availableCubes(12, 13, 14);

    std::string definition;
    while (std::getline(input, definition))
    {
        Game game = makeGame(definition);
        if (game.isPlayableWith(availableCubes))
        {
            sumOfIds += game.getId();
        }
    }

    CHECK_EQUAL(2239, sumOfIds);
}

TEST(Solution, part2)
{
    uint32_t sumOfPowers = 0;

    std::string definition;
    while (std::getline(input, definition))
    {
        sumOfPowers += makeGame(definition).getMinimumSetOfCubes().power();
    }

    CHECK_EQUAL(83435, sumOfPowers);
}
}
