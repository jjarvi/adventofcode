#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace day07 {

class Worker
{
public:
    Worker(char item, int baseTime) :
        item(item),
        timeLeft(baseTime + item - 'A' + 1)
    {
    }

    void tick()
    {
        timeLeft--;
    }

    bool isReady() const
    {
        return timeLeft <= 0;
    }

    char getItem() const
    {
        return item;
    }

private:
    char item;
    int timeLeft;
};


class Scheduler
{
public:
    Scheduler(unsigned int numOfWorkers, int baseTime) :
        maxNumOfWorkers(numOfWorkers),
        baseTime(baseTime)
    {
        workers.reserve(maxNumOfWorkers);
    }

    void add(char item)
    {
        if (workers.size() >= maxNumOfWorkers)
        {
            return;
        }
        for (auto& w : workers)
        {
            if (w.getItem() == item)
            {
                return;
            }
        }
        workers.push_back(Worker(item, baseTime));
    }

    void tick()
    {
        finished.clear();

        for (auto& w : workers)
        {
            w.tick();
            if (w.isReady())
            {
                finished.insert(w.getItem());
            }
        }

        workers.erase(std::remove_if(workers.begin(), workers.end(),
            [](const Worker& w) {return w.isReady();}), workers.end());
    }

    const std::set<char>& getFinished() const
    {
        return finished;
    }

private:
    std::set<char> finished;
    std::vector<Worker> workers;
    unsigned int maxNumOfWorkers;
    int baseTime;
};


static void removeStep(char step, std::map<char, std::set<char>>& steps)
{
    for (auto& s : steps)
    {
        s.second.erase(step);
    }
    steps.erase(step);
}


static void removeSteps(const std::set<char>& remove, std::map<char, std::set<char>>& steps)
{
    for (auto r : remove)
    {
        removeStep(r, steps);
    }
}


static std::string sortSteps(std::map<char, std::set<char>> steps)
{
    std::string answer;
    
    while (steps.size() > 0)
    {
        std::set<char> candidates;
        for (auto& s : steps)
        {
            if (s.second.size() == 0)
            {
                candidates.insert(s.first);
            }
        }

        char c = *candidates.begin();
        answer += c;
        removeStep(c, steps);
    }

    return answer;
}


static int processInParallel(std::map<char, std::set<char>> steps, int numOfWorkers, int baseTime)
{
    int ticks = 0;
    Scheduler scheduler(numOfWorkers, baseTime);

    while (true)
    {
        removeSteps(scheduler.getFinished(), steps);

        if (steps.size() == 0)
        {
            break;
        }

        for (auto& s : steps)
        {
            if (s.second.size() == 0)
            {
                scheduler.add(s.first);
            }
        }
        scheduler.tick();
        ticks++;
    }

    return ticks;
}


static bool isSpace(char c)
{
    return c == ' ';
}


static std::pair<char, char> parseLine(const std::string& line)
{
    static constexpr unsigned int validLineLength = 48;
    static constexpr unsigned int dependencyPos = 5;
    static constexpr unsigned int stepPos = 36;
    static constexpr char NA = '-';
    
    if (line.length() == validLineLength &&
        isSpace(line[dependencyPos - 1]) && isSpace(line[dependencyPos + 1]) &&
        isSpace(line[stepPos - 1]) && isSpace(line[stepPos + 1]))
    {
        return {line[dependencyPos], line[stepPos]};
    }
    std::cerr << "Invalid input: " << line << std::endl;
    return {NA, NA};
}


static std::map<char, std::set<char>> parseInput(std::istream& input)
{
    std::map<char, std::set<char>> steps;
    std::string line;
    while (std::getline(input, line))
    {
        auto s = parseLine(line);
        if (steps.count(s.first) == 0)
        {
            steps[s.first] = {};
        }
        steps[s.second].insert(s.first);
    }
    return steps;
}


class Day07Example : public ::testing::Test
{
public:
    Day07Example() :
        inputText(""
            "Step C must be finished before step A can begin.\n"
            "Step C must be finished before step F can begin.\n"
            "Step A must be finished before step B can begin.\n"
            "Step A must be finished before step D can begin.\n"
            "Step B must be finished before step E can begin.\n"
            "Step D must be finished before step E can begin.\n"
            "Step F must be finished before step E can begin."),
        input(inputText)
    {
    }

protected:
    std::string inputText;
    std::stringstream input;
};


TEST_F(Day07Example, first)
{
    EXPECT_EQ("CABDFE", sortSteps(parseInput(input)));
}


TEST_F(Day07Example, second)
{
    EXPECT_EQ(15, processInParallel(parseInput(input), 2, 0));
}


TEST(Day07, solution)
{
    std::ifstream input("../day07_input.txt");
    std::map<char, std::set<char>> steps = parseInput(input);
    EXPECT_EQ("EUGJKYFQSCLTWXNIZMAPVORDBH", sortSteps(steps));
    EXPECT_EQ(1014, processInParallel(steps, 5, 60));
}

}
