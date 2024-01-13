#include <gtest/gtest.h>

#include <vector>
#include <limits>
#include <queue>
#include <memory>
#include <stack>
#include <fstream>

namespace day15 {


class Point
{
public:
    Point()
    {
        invalidate();
    }

    Point(int x, int y) :
        x(x),
        y(y)
    {
    }

    virtual ~Point()
    {
    }

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return !(*this == other);
    }

    Point& operator=(const Point& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    void invalidate()
    {
        x = getInvalidValue();
        y = getInvalidValue();
    }

    bool isValid() const
    {
        return x != getInvalidValue() && y != getInvalidValue();
    }

    static int getInvalidValue()
    {
        return std::numeric_limits<int>::max();
    }

    int x;
    int y;
};

Point operator+(Point lhs, const Point& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}



class Unit : public Point
{
public:
    enum Type
    {
        Goblin = 0,
        Elf,
        Unknown
    };

    static constexpr int maxHP = 200;
    static constexpr int normalDamage = 3;

    Unit(Type type = Unit::Unknown, int x = 0, int y = 0) :
        Point(x, y),
        type(type),
        hp(maxHP)
    {
    }

    int getDamage() const
    {
        return damage[type];
    }

    static void setDamage(Type type, int d)
    {
        assert(type < Type::Unknown);
        damage[type] = d;
    }

    bool operator<(const Unit& other) const
    {
        return y < other.y || (y == other.y && x < other.x);
    }

    bool isAlive() const
    {
        return hp > 0;
    }

    bool isDead() const
    {
        return !isAlive();
    }

    bool isFriend(const Unit& other) const
    {
        return type == other.type;
    }

    Type type;
    int hp;

private:
    static int damage[];
};

int Unit::damage[] = { Unit::normalDamage, Unit::normalDamage };
constexpr int Unit::maxHP;


class Node : public Point
{
public:
    enum Type
    {
        Floor,
        Wall
    };

    Node(int x = 0, int y = 0) :
        Point(x, y),
        parent(),
        unit(nullptr),
        type(Floor),
        visited(false)
    {
    }

    Node(int x, int y, Type type, Unit* unit) :
        Point(x, y),
        parent(),
        unit(unit),
        type(type),
        visited(false)
    {
    }

    virtual ~Node()
    {
    }

    bool hasUnit() const
    {
        return unit != nullptr;
    }

    Point parent;
    Unit* unit;
    Type type;
    bool visited;
};

using UnitList = std::vector<Unit*>;
using Route = std::stack<Point>;
using Neighbors = std::array<Point, 4>;


class Map
{
public:

    Map(const std::string input)
    {
        std::stringstream ss(input);
        read(ss);
    }

    Map(std::istream& input)
    {
        read(input);
    }

    Map(const Map& other)
    {
        nodes = other.nodes;
        units = other.units;

        for (size_t i = 0; i < MAX_NUM_OF_UNITS; ++i)
        {
            if (units[i].type != Unit::Unknown)
            {
                activeUnits.push_back(&units[i]);
                getNode(units[i]).unit = &units[i];
            }
        }
    }

    UnitList& getUnits()
    {
        return activeUnits;
    }

    std::vector<Node>& operator[](size_t x)
    {
        return nodes.at(x);
    }

    bool moveToNearestEnemy(Unit* unit)
    {
        std::vector<Route> routes;
        routes.reserve(activeUnits.size());

        for (auto& enemy : activeUnits)
        {
            if (!enemy->isDead() && !unit->isFriend(*enemy))
            {
                routes.emplace_back(calculateRouteToEnemy(unit, *enemy));
            }
        }

        if (routes.size() > 0)
        {
            auto shortest = selectShortestRoute(routes);
            if (shortest != routes.end())
            {
                moveUnit(unit, shortest->top());
            }
            return true;
        }
        return false;  // no enemies left, game over
    }

    Unit* findEnemyInRange(const Unit& unit) const
    {
        Unit* chosen = nullptr;
        for (auto& neighbor : getNeighbors(unit))
        {
            if (isFloor(neighbor) &&
                hasUnit(neighbor) &&
                getNode(neighbor).unit->isFriend(unit) == false &&
                getNode(neighbor).unit->isAlive())
            {
                if (!chosen || (chosen && chosen->hp > getNode(neighbor).unit->hp))
                {
                    chosen = getNode(neighbor).unit;
                }
            }
        }
        return chosen;
    }

    const Node& getNode(const Point& p) const
    {
        return nodes[p.x][p.y];
    }

    Node& getNode(const Point& p)
    {
        return nodes[p.x][p.y];
    }


private:
    void read(std::istream& input)
    {
        std::string line;
        int y = 0;
        while (std::getline(input, line))
        {
            if (nodes.size() == 0)
            {
                nodes.resize(line.length());
            }
            else if (nodes.size() != line.length())
            {
                throw std::runtime_error("Invalid Map");
            }

            int x = 0;
            for (auto c : line)
            {
                Unit* unit = nullptr;
                Node::Type type = Node::Floor;

                if (isGoblin(c))
                {
                    unit = createUnit(Unit::Goblin, x, y);
                }
                else if (isElf(c))
                {
                    unit = createUnit(Unit::Elf, x, y);
                }
                else if (isWall(c))
                {
                    type = Node::Wall;
                }
                nodes[x].emplace_back(x, y, type, unit);
                x++;
            }
            y++;
        }
    }

    bool isFloor(const Point& p ) const
    {
        return p.x >= 0 && p.y >= 0 &&
            p.x < static_cast<int>(nodes.size()) &&
            p.y < static_cast<int>(nodes[p.x].size()) &&
            getNode(p).type == Node::Floor;
    }

    bool hasUnit(const Point& p) const
    {
        return getNode(p).unit != nullptr;
    }

    bool isWalkable(const Point& p) const
    {
        return isFloor(p) && !hasUnit(p);
    }

    const Neighbors& getNeighbors(const Point& point) const
    {
        static const Neighbors offsets = {
            Point( 0, -1),
            Point(-1,  0),
            Point( 1,  0),
            Point( 0,  1)
        };

        static Neighbors neighbors;
        for (size_t i = 0; i < offsets.size(); ++i)
        {
            neighbors[i] = point + offsets[i];
        }
        return neighbors;
    }

    std::vector<Route>::iterator selectShortestRoute(std::vector<Route>& routes)
    {
        auto shortest = routes.end();
        for (auto r = routes.begin(); r != routes.end(); ++r)
        {
            if (r->size() == 0)  // Already in range of an enemy
            {
                continue;
            }

            if (shortest == routes.end() ||
                (shortest != routes.end() && r->size() < shortest->size()))
            {
                shortest = r;
            }
        }
        return shortest;
    }

    void moveUnit(Unit* unit, const Point& to)
    {
        getNode(to).unit = unit;
        getNode(*unit).unit = nullptr;
        unit->x = to.x;
        unit->y = to.y;
    }

    Route calculateRouteToEnemy(Unit* unit, const Point& target)
    {
        for (auto& v : nodes)
        {
            for (auto& n : v)
            {
                n.visited = false;
            }
        }

        Route route;
        std::queue<Node> heads;
        heads.push(nodes[unit->x][unit->y]);
        while (!heads.empty())
        {
            Node current = heads.front();
            heads.pop();
            if (current == getNode(target))
            {
                return buildRoute(target, *unit);
            }
            const auto& neighbors = getNeighbors(current);
            for (const auto& nb : neighbors)
            {
                if (!isFloor(nb))
                {
                    continue;
                }
                Node& neighbor = getNode(nb);
                if (neighbor.hasUnit() && unit->isFriend(*neighbor.unit))
                {
                    continue;
                }
                if (!neighbor.visited)
                {
                    neighbor.visited = true;
                    neighbor.parent = current;
                    heads.push(neighbor);
                }
            }
        }
        return route;
    }

    Route buildRoute(const Point& from, const Point& to) const
    {
        Route route;
        const Node* next = &getNode(from);
        while(next->parent.isValid() && next->parent != to)
        {
            route.push(getNode(next->parent));
            next = &getNode(next->parent);
        }
        return route;
    }

    bool isGoblin(char c)
    {
        return c == 'G';
    }

    bool isElf(char c)
    {
        return c == 'E';
    }

    bool isWall(char c)
    {
        return c == '#';
    }

    Unit* createUnit(Unit::Type type, int x, int y)
    {
        assert(activeUnits.size() < MAX_NUM_OF_UNITS);
        Unit* unit = &units[activeUnits.size()];
        unit->type = type;
        unit->x = x;
        unit->y = y;
        activeUnits.push_back(unit);
        return unit;
    }

    std::vector<std::vector<Node>> nodes;
    UnitList activeUnits;
    static constexpr unsigned int MAX_NUM_OF_UNITS = 32;
    std::array<Unit, MAX_NUM_OF_UNITS> units;
};


class Game
{
public:
    Game(Map& map) :
        map(map),
        round(0),
        gameOver(false)
    {
    }

    void run()
    {
        while(tick());
    }

    bool tick()
    {
        auto& units = map.getUnits();
        sortUnitsByPosition(units);

        for (auto& unit : units)
        {
            turn(unit);
        }
        if (!gameOver)
        {
            round++;
        }
        return !gameOver;
    }

    long int getOutcome() const
    {
        long int hp = 0;
        for (const auto& unit : map.getUnits())
        {
            if (unit->isAlive())
            {
                hp += unit->hp;
            }
        }
        return round * hp;
    }


private:
    Map& map;
    int round;
    bool gameOver;

    void sortUnitsByPosition(UnitList& units) const
    {
        std::sort(units.begin(), units.end(),
            [](const Unit* lhs, const Unit* rhs) {
                return *lhs < *rhs;
                });
    }

    void attack(Unit& unit, Unit& enemy)
    {
        enemy.hp -= unit.getDamage();
        if (enemy.isDead())
        {
            map.getNode(enemy).unit = nullptr;
        }
    }

    bool attackIfEnemyInRange(Unit& unit)
    {
        Unit* enemy = map.findEnemyInRange(unit);
        if (enemy)
        {
            attack(unit, *enemy);
            return true;
        }
        return false;
    }

    void turn(Unit* unit)
    {
        if (unit->isDead())
        {
            return;
        }
        if (attackIfEnemyInRange(*unit))
        {
            return;
        }
        if(map.moveToNearestEnemy(unit))
        {
            attackIfEnemyInRange(*unit);
        }
        else
        {
            gameOver = true;
        }
    }
};



TEST(Day15, mapParsing)
{
    Map map(
        "#####\n"
        "#G#.#\n"
        "#..E#\n"
        "#####\n");

    EXPECT_EQ(0, map[0][0].x);
    EXPECT_EQ(0, map[0][0].y);
    EXPECT_EQ(Node::Wall, map[0][0].type);
    EXPECT_EQ(nullptr, map[0][0].unit);

    EXPECT_EQ(1, map[1][0].x);
    EXPECT_EQ(0, map[0][0].y);
    EXPECT_EQ(Node::Wall, map[1][0].type);
    EXPECT_EQ(nullptr, map[1][0].unit);

    EXPECT_EQ(1, map[1][1].x);
    EXPECT_EQ(1, map[1][1].y);
    EXPECT_EQ(Node::Floor, map[1][1].type);
    ASSERT_NE(nullptr, map[1][1].unit);
    EXPECT_EQ(Unit::Goblin, map[1][1].unit->type);

    EXPECT_EQ(1, map[1][2].x);
    EXPECT_EQ(2, map[1][2].y);
    EXPECT_EQ(Node::Floor, map[1][2].type);
    EXPECT_EQ(nullptr, map[1][2].unit);

    EXPECT_EQ(3, map[3][2].x);
    EXPECT_EQ(2, map[3][2].y);
    EXPECT_EQ(Node::Floor, map[3][2].type);
    ASSERT_NE(nullptr, map[3][2].unit);
    EXPECT_EQ(Unit::Elf, map[3][2].unit->type);
}

TEST(Day15, unitsAttackEnemiesInRange)
{
    Map map(
        "#######\n"
        "#.....#\n"
        "#.GE..#\n"
        "#.E...#\n"
        "#######\n");
    Game game(map);
    auto& units = map.getUnits();

    auto assertPositions = [&]() {
        EXPECT_EQ(2, units[0]->x);
        EXPECT_EQ(2, units[0]->y);
        EXPECT_EQ(3, units[1]->x);
        EXPECT_EQ(2, units[1]->y);
        EXPECT_EQ(2, units[2]->x);
        EXPECT_EQ(3, units[2]->y);
    };

    assertPositions();
    EXPECT_EQ(Unit::maxHP, units[0]->hp);
    EXPECT_EQ(Unit::maxHP, units[1]->hp);
    EXPECT_EQ(Unit::maxHP, units[2]->hp);
    EXPECT_TRUE(game.tick());  // not game over yet
    assertPositions();
    EXPECT_EQ(Unit::maxHP - units[1]->getDamage() - units[2]->getDamage(), units[0]->hp);
    EXPECT_EQ(Unit::maxHP - units[0]->getDamage(), units[1]->hp);
    EXPECT_EQ(Unit::maxHP, units[2]->hp);
    EXPECT_TRUE(game.tick());  // not game over yet
    assertPositions();
    EXPECT_EQ(Unit::maxHP - 2 * units[1]->getDamage() - 2 * units[2]->getDamage(), units[0]->hp);
    EXPECT_EQ(Unit::maxHP - 2 * units[0]->getDamage(), units[1]->hp);
    EXPECT_EQ(Unit::maxHP, units[2]->hp);
}


TEST(Day15, gameEndsWhenAllEnemiesAreDead)
{
    Map map(
        "#######\n"
        "#.....#\n"
        "#.GE..#\n"
        "#.E...#\n"
        "#######\n");
    Game game(map);
    auto& units = map.getUnits();

    game.run();
    EXPECT_TRUE(units[0]->isDead());
    EXPECT_EQ(Unit::maxHP - 34 * units[0]->getDamage(), units[1]->hp);
    EXPECT_EQ(Unit::maxHP, units[2]->hp);
}

TEST(Day15, outcomeOfTheGame)
{
    Map map(
        "#######\n"
        "#E.G#.#\n"
        "#.#G..#\n"
        "#G.#.G#\n"
        "#G..#.#\n"
        "#...E.#\n"
        "#######\n");
    Game game(map);
    game.run();
    // Goblins win after 35 rounds with 793 hp left. 35 * 793 = 27755
    EXPECT_EQ(35 * 793, game.getOutcome());
}

TEST(Day15, simplePathFinding)
{
    Map map(
        //1234567
        "#########\n"
        "#...#...#\n" // 1
        "#...#...#\n" // 2
        "#...#...#\n" // 3
        "#...#...#\n" // 4
        "#..E#...#\n" // 5
        "#.###G..#\n" // 6
        "#...###.#\n" // 7
        "#.......#\n" // 8
        "#########\n");
    Game game(map);
    auto& units = map.getUnits();
    Unit* elf = units[0];
    Unit* goblin = units[1];

    ASSERT_EQ(3, elf->x);
    ASSERT_EQ(5, elf->y);
    ASSERT_EQ(5, goblin->x);
    ASSERT_EQ(6, goblin->y);
    ASSERT_TRUE(game.tick());
    ASSERT_EQ(2, elf->x);
    ASSERT_EQ(5, elf->y);
    ASSERT_EQ(6, goblin->x);
    ASSERT_EQ(6, goblin->y);
    ASSERT_TRUE(game.tick());
    EXPECT_EQ(1, elf->x);
    EXPECT_EQ(5, elf->y);
    EXPECT_EQ(7, goblin->x);
    EXPECT_EQ(6, goblin->y);
    ASSERT_TRUE(game.tick());
    EXPECT_EQ(1, elf->x);
    EXPECT_EQ(6, elf->y);
    EXPECT_EQ(7, goblin->x);
    EXPECT_EQ(7, goblin->y);
    ASSERT_TRUE(game.tick());
    EXPECT_EQ(1, elf->x);
    EXPECT_EQ(7, elf->y);
    EXPECT_EQ(7, goblin->x);
    EXPECT_EQ(8, goblin->y);
    ASSERT_TRUE(game.tick());
    EXPECT_EQ(2, elf->x);
    EXPECT_EQ(7, elf->y);
    EXPECT_EQ(6, goblin->x);
    EXPECT_EQ(8, goblin->y);
    ASSERT_TRUE(game.tick());
    EXPECT_EQ(3, elf->x);
    EXPECT_EQ(7, elf->y);
    EXPECT_EQ(5, goblin->x);
    EXPECT_EQ(8, goblin->y);
    ASSERT_TRUE(game.tick()); // attacks
    EXPECT_EQ(3, elf->x);
    EXPECT_EQ(8, elf->y);
    EXPECT_EQ(4, goblin->x);
    EXPECT_EQ(8, goblin->y);
    ASSERT_TRUE(game.tick()); // should stay at same position
    EXPECT_EQ(3, elf->x);
    EXPECT_EQ(8, elf->y);
    EXPECT_EQ(4, goblin->x);
    EXPECT_EQ(8, goblin->y);
}

TEST(Day15, unitMovesOnlyAlongOpenPath)
{
    Map map(
        //12345
        "######\n"
        "#..E.#\n" // 1
        "#....#\n" // 2
        "#..E.#\n" // 3
        "#....#\n" // 4
        "###.##\n" // 5
        "#G...#\n" // 6
        "######\n");
    Game game(map);
    auto& units = map.getUnits();
    Unit* elf1 = units[0];
    Unit* elf2 = units[1];
    Unit* goblin = units[2];

    ASSERT_EQ(3, elf1->x);
    ASSERT_EQ(1, elf1->y);
    ASSERT_EQ(3, elf2->x);
    ASSERT_EQ(3, elf2->y);
    ASSERT_EQ(1, goblin->x);
    ASSERT_EQ(6, goblin->y);
    ASSERT_TRUE(game.tick());
    ASSERT_EQ(2, elf1->x);
    ASSERT_EQ(1, elf1->y);
    ASSERT_EQ(3, elf2->x);
    ASSERT_EQ(4, elf2->y);
    ASSERT_EQ(2, goblin->x);
    ASSERT_EQ(6, goblin->y);
    ASSERT_TRUE(game.tick());
    ASSERT_EQ(2, elf1->x);      // Elf 1 stays on 2.1 because there's no open path
    ASSERT_EQ(1, elf1->y);      // towards goblin
    ASSERT_EQ(3, elf2->x);
    ASSERT_EQ(5, elf2->y);
    ASSERT_EQ(3, goblin->x);
    ASSERT_EQ(6, goblin->y);
}

TEST(Day15, unitsMoveTowardsNearestEnemy)
{
    Map map(
        //123456789
        "###########\n"
        "#E.....G.E#\n" // 1
        "###########\n");
    Game game(map);
    auto& units = map.getUnits();
    Unit* elf1 = units[0];
    Unit* goblin = units[1];
    Unit* elf2 = units[2];

    ASSERT_EQ(1, elf1->x);
    ASSERT_EQ(1, elf1->y);
    ASSERT_EQ(7, goblin->x);
    ASSERT_EQ(1, goblin->y);
    ASSERT_EQ(9, elf2->x);
    ASSERT_EQ(1, elf2->y);
    ASSERT_TRUE(game.tick());
    ASSERT_EQ(2, elf1->x);
    ASSERT_EQ(1, elf1->y);
    ASSERT_EQ(8, goblin->x);
    ASSERT_EQ(1, goblin->y);
    ASSERT_EQ(9, elf2->x);  // goblin moves to 8.1 before it's elf2's turn
    ASSERT_EQ(1, elf2->y);
    ASSERT_TRUE(game.tick());
}

TEST(Day15, unitComparisonInVerticalDirection)
{
    Unit a(Unit::Goblin, 0, 1);
    Unit b(Unit::Goblin, 0, 0);
    Unit c(Unit::Goblin, 3, 0);
    ASSERT_TRUE(b < a);
    ASSERT_FALSE(a < b);
    ASSERT_TRUE(c < a);
    ASSERT_FALSE(a < c);
}

TEST(Day15, unitComparisonInHorizontalDirection)
{
    Unit a(Unit::Goblin, 1, 0);
    Unit b(Unit::Goblin, 0, 0);
    ASSERT_TRUE(b < a);
    ASSERT_FALSE(a < b);
}

TEST(Day15, unitsAreSortedByPosition)
{
    std::set<Unit> units;
    units.insert(Unit(Unit::Goblin, 2, 2));
    units.insert(Unit(Unit::Elf, 0, 0));
    units.insert(Unit(Unit::Goblin, 1, 2));

    auto it = units.begin();
    EXPECT_EQ(0, it->x);
    EXPECT_EQ(0, it->y);
    it++;
    EXPECT_EQ(1, it->x);
    EXPECT_EQ(2, it->y);
    it++;
    EXPECT_EQ(2, it->x);
    EXPECT_EQ(2, it->y);
}

TEST(Day15, attackOrder)
{
    Map map(
        "#####\n"
        "#.G.#\n"
        "#GEG#\n"
        "#.G.#\n"
        "#####\n");
    Game game(map);
    auto& units = map.getUnits();
    game.tick();

    // All goblins had same HP so elf attacked the first in reading order
    // All goblins attacked the elf
    ASSERT_EQ(Unit::maxHP - units[2]->getDamage(),     units[0]->hp);
    ASSERT_EQ(Unit::maxHP,                             units[1]->hp);
    ASSERT_EQ(Unit::maxHP - units[0]->getDamage() * 4, units[2]->hp);
    ASSERT_EQ(Unit::maxHP,                             units[3]->hp);
    ASSERT_EQ(Unit::maxHP,                             units[4]->hp);

    units[4]->hp = 50;
    game.tick();

    // Goblin at the bottom had lowest HP so it was preferred over the first in reading order
    // All goblins attacked the elf
    ASSERT_EQ(Unit::maxHP - units[2]->getDamage(),         units[0]->hp);
    ASSERT_EQ(Unit::maxHP,                                 units[1]->hp);
    ASSERT_EQ(Unit::maxHP - units[0]->getDamage() * 4 * 2, units[2]->hp);
    ASSERT_EQ(Unit::maxHP,                                 units[3]->hp);
    ASSERT_EQ(47,                                          units[4]->hp);
}

TEST(Day15, example1)
{
    Map map(
        "#######\n"
        "#.G...#\n"
        "#...EG#\n"
        "#.#.#G#\n"
        "#..G#E#\n"
        "#.....#\n"
        "#######\n");
    Game game(map);
    auto& units = map.getUnits();
    for (int i = 0; i < 47; ++i)
    {
        ASSERT_TRUE(game.tick());
    }
    // Round 48, no enemies left
    ASSERT_FALSE(game.tick());

    ASSERT_EQ(Unit::Goblin, units[0]->type);
    ASSERT_EQ(1, units[0]->x);
    ASSERT_EQ(1, units[0]->y);
    ASSERT_EQ(200, units[0]->hp);

    ASSERT_EQ(Unit::Goblin, units[1]->type);
    ASSERT_EQ(2, units[1]->x);
    ASSERT_EQ(2, units[1]->y);
    ASSERT_EQ(131, units[1]->hp);

    // dead elf is at units[2]

    ASSERT_EQ(Unit::Goblin, units[3]->type);
    ASSERT_EQ(5, units[3]->x);
    ASSERT_EQ(3, units[3]->y);
    ASSERT_EQ(59, units[3]->hp);

    ASSERT_EQ(27730, game.getOutcome());
}

TEST(Day15, example2)
{
    Map map(
        "#######\n"
        "#G..#E#\n"
        "#E#E.E#\n"
        "#G.##.#\n"
        "#...#E#\n"
        "#...E.#\n"
        "#######\n");
    Game game(map);
    game.run();
    EXPECT_EQ(36334, game.getOutcome());
}

TEST(Day15, part1Solution)
{
    std::ifstream input("../day15_input.txt");
    Map map(input);
    Game game(map);
    game.run();
    EXPECT_EQ(263327, game.getOutcome());
}


TEST(Day15, part2Solution)
{
    auto didElvesWin = [](const UnitList& units) {
        for (const auto& unit : units)
        {
            if (unit->type == Unit::Elf && unit->isDead())
            {
                return false;
            }
        }
        return true;
    };

    int elfDamage = Unit::normalDamage + 1;
    std::ifstream input("../day15_input.txt");
    Map scenario(input);

    while (true)
    {
        Map map(scenario);
        Game game(map);
        game.run();
        if (didElvesWin(map.getUnits()))
        {
            EXPECT_EQ(77872, game.getOutcome());
            break;
        }
        Unit::setDamage(Unit::Elf, ++elfDamage);
    }
}

}
