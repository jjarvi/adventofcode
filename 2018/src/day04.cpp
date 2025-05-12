#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

namespace day04 {

enum class Action
{
    BeginShift,
    FallAsleep,
    WakeUp
};


class Guard
{
public:
    static const int INVALID_ID = -1;

    Guard() :
        isAsleep(false)
    {
    }

    static bool isValidId(int id)
    {
        return id > INVALID_ID;
    }

    int getTotalSleepMinutes() const
    {
        int minutes = 0;

        for (auto& s : sleeptimes)
        {
            minutes += (int)std::difftime(s.second, s.first) / 60;
        }

        return minutes;
    }

    int getMostSleptMinute() const
    {
        return getMostSleptMinuteAndCount().first;
    }

    std::pair<int, int> getMostSleptMinuteAndCount() const
    {
        std::map<int, int> minutes;
        for (auto& s : sleeptimes)
        {
            auto cur = std::chrono::system_clock::from_time_t(s.first);
            auto end = std::chrono::system_clock::from_time_t(s.second);
            while (cur < end)
            {
                time_t t = std::chrono::system_clock::to_time_t(cur);
                std::tm* tm = std::localtime(&t);
                minutes[tm->tm_min]++;
                cur += std::chrono::minutes(1);
            }
        }

        std::pair<int, int> ret;
        for (auto& m : minutes)
        {
            if (m.second > ret.second)
            {
                ret = m;
            }
        }
        return ret;
    }

    void sleep(const time_t timestamp)
    {
        assert(!isAsleep);
        isAsleep = true;
        sleeptimes.push_back({timestamp, 0});
    }

    void wakeUp(const time_t timestamp)
    {
        assert(isAsleep);
        isAsleep = false;
        sleeptimes.back().second = timestamp;
    }


private:
    bool isAsleep;
    std::vector<std::pair<time_t, time_t>> sleeptimes;
};


class LogEntry
{
public:
    LogEntry(const std::string& line) :
        guardId(Guard::INVALID_ID)
    {
        setTimestamp(line);
        setAction(line);
    }

    bool operator<(const LogEntry& other) const
    {
        return timestamp < other.timestamp;
    }

    time_t getTimestamp() const
    {
        return timestamp;
    }

    Action getAction() const
    {
        return action;
    }

    int getGuardId() const
    {
        return guardId;
    }

private:
    void setTimestamp(const std::string& str)
    {
        std::stringstream ss(applyYearWorkaround(str.substr(1)));
        std::tm t = {};
        ss >> std::get_time(&t, "%Y-%m-%d %H:%M");
        timestamp = std::mktime(&t);
    }

    // negative time_t represents error
    std::string applyYearWorkaround(const std::string& str)
    {
        std::string fixed = str;
        fixed.replace(0, 2, "20");
        return fixed;
    }

    void setAction(const std::string& str)
    {
        if (str.find("begins shift") != std::string::npos)
        {
            action = Action::BeginShift;
            setGuardId(str);
        }
        else if (str.find("falls asleep") != std::string::npos)
        {
            action = Action::FallAsleep;
        }
        else
        {
            action = Action::WakeUp;
        }
    }

    void setGuardId(const std::string& str)
    {
        size_t begin = str.find('#');
        size_t end = str.find(' ', begin);
        if (begin != std::string::npos && end != std::string::npos && begin + 1 < end)
        {
            guardId = std::stoi(str.substr(begin + 1, end - begin));
        }
    }

    time_t timestamp;
    Action action;
    int guardId;
};


class GuardLog
{
public:
    void parse(const std::vector<LogEntry>& entries)
    {
        guard = nullptr;
        for (auto& entry : entries)
        {
            parseEntry(entry);
        }
    }

    const std::map<int, Guard>& getGuards() const
    {
        return guards;
    }


    int whoSleptMost() const
    {
        int id = Guard::INVALID_ID;
        int minutes = 0;
        for (auto& g : guards)
        {
            int total = g.second.getTotalSleepMinutes();
            if (total > minutes)
            {
                minutes = total;
                id = g.first;
            }
        }
        return id;
    }


    int whoIsMostFrequentlySleepingOnSameMinute() const
    {
        int id = Guard::INVALID_ID;
        int count = 0;
        for (auto& g : guards)
        {
            std::pair<int, int> m = g.second.getMostSleptMinuteAndCount();
            if (m.second > count)
            {
                count = m.second;
                id = g.first;
            }
        }
        return id;
    }


private:
    void parseEntry(const LogEntry& entry)
    {
        switch (entry.getAction())
        {
            case Action::BeginShift:
                onBeginShift(entry);
                break;

            case Action::FallAsleep:
                onFallAsleep(entry);
                break;

            default:
                onWakeUp(entry);
                break;
        }
    }

    void onBeginShift(const LogEntry& entry)
    {
        int id = entry.getGuardId();
        if (Guard::isValidId(id))
        {
            guard = &guards[id];
        }
    }

    void onFallAsleep(const LogEntry& entry)
    {
        if (guard)
        {
            guard->sleep(entry.getTimestamp());
        }
    }

    void onWakeUp(const LogEntry& entry)
    {
        if (guard)
        {
            guard->wakeUp(entry.getTimestamp());
        }
    }

    std::map<int, Guard> guards;
    Guard* guard;
};


class Day04Solution : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        std::ifstream input(puzzleInputs::getInputDirectory() / "day04_input.txt");
        EXPECT_TRUE(input.is_open());
        std::string line;
        std::vector<LogEntry> entries;
        while (std::getline(input, line))
        {
            entries.push_back(LogEntry(line));
        }
        std::sort(entries.begin(), entries.end());
        log.parse(entries);
    }

    static GuardLog log;
};

GuardLog Day04Solution::log;


class GuardTest : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        log.parse({
            LogEntry("[1518-11-01 00:00] Guard #10 begins shift"),
            LogEntry("[1518-11-01 00:05] falls asleep"),
            LogEntry("[1518-11-01 00:25] wakes up"),
            LogEntry("[1518-11-01 00:30] falls asleep"),
            LogEntry("[1518-11-01 00:55] wakes up"),
            LogEntry("[1518-11-01 23:58] Guard #9 begins shift"),
            LogEntry("[1518-11-02 00:40] falls asleep"),
            LogEntry("[1518-11-02 00:50] wakes up"),
            LogEntry("[1518-11-03 00:05] Guard #10 begins shift"),
            LogEntry("[1518-11-03 00:24] falls asleep"),
            LogEntry("[1518-11-03 00:29] wakes up"),
            LogEntry("[1518-11-04 00:02] Guard #9 begins shift"),
            LogEntry("[1518-11-04 00:36] falls asleep"),
            LogEntry("[1518-11-04 00:46] wakes up"),
            LogEntry("[1518-11-05 00:03] Guard #9 begins shift"),
            LogEntry("[1518-11-05 00:45] falls asleep"),
            LogEntry("[1518-11-05 00:55] wakes up")
        });
    }

    static GuardLog log;
};

GuardLog GuardTest::log;


TEST(Day04_LogEntry, action_beginShift)
{
    EXPECT_EQ(Action::BeginShift, LogEntry("[1518-01-01 12:00] Guard #15 begins shift").getAction());
}

TEST(Day04_LogEntry, action_fallAsleep)
{
    EXPECT_EQ(Action::FallAsleep, LogEntry("[1518-11-01 00:05] falls asleep").getAction());
}

TEST(Day04_LogEntry, action_wakeUp)
{
    EXPECT_EQ(Action::WakeUp, LogEntry("[1518-11-01 00:25] wakes up").getAction());
}

TEST(Day04_LogEntry, compare_minutes)
{
    EXPECT_LT(LogEntry("[1518-11-01 00:00] Guard #15 begins shift"),
              LogEntry("[1518-11-01 00:01] falls asleep"));
}

TEST(Day04_LogEntry, compare_hour)
{
    EXPECT_LT(LogEntry("[1518-11-01 00:00] Guard #15 begins shift"),
              LogEntry("[1518-11-01 01:00] falls asleep"));
}

TEST(Day04_LogEntry, compare_day)
{
    EXPECT_LT(LogEntry("[1518-11-01 00:00] Guard #15 begins shift"),
              LogEntry("[1518-11-02 00:00] falls asleep"));
}

TEST(Day04_LogEntry, compare_month)
{
    EXPECT_LT(LogEntry("[1518-11-01 00:00] Guard #15 begins shift"),
              LogEntry("[1518-12-01 00:00] falls asleep"));
}

TEST(Day04_LogEntry, compare_year)
{
    EXPECT_LT(LogEntry("[1518-11-01 00:00] Guard #15 begins shift"),
              LogEntry("[1519-11-01 00:00] falls asleep"));
}

TEST_F(GuardTest, totalSleepMinutes)
{
    EXPECT_EQ(10 + 10 + 10, log.getGuards().at(9).getTotalSleepMinutes());
    EXPECT_EQ(20 + 25 + 5, log.getGuards().at(10).getTotalSleepMinutes());
}

TEST_F(GuardTest, mostSleptMinute)
{
    EXPECT_EQ(24, log.getGuards().at(10).getMostSleptMinute());
}

TEST_F(Day04Solution, part1)
{
    EXPECT_EQ(179, log.whoSleptMost());
    EXPECT_EQ(50, log.getGuards().at(179).getMostSleptMinute());
    EXPECT_EQ(8950, 179 * 50);
}

TEST_F(Day04Solution, part2)
{
    int id = log.whoIsMostFrequentlySleepingOnSameMinute();
    EXPECT_EQ(1783, id);
    EXPECT_EQ(44, log.getGuards().at(id).getMostSleptMinute());
    EXPECT_EQ(78452, 1783 * 44);
}

}
