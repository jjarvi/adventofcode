#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include <array>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <filesystem>

#include <gtest/gtest.h>

#include "PuzzleInputs.hpp"

namespace aoc2018::day16 {

enum class OpCode : int
{
    // (add register) stores into register C the result of adding register A and register B
    addr = 0,

    // addi (add immediate) stores into register C the result of adding register A and value B
    addi,

    // (multiply register) stores into register C the result of multiplying register A and register B
    mulr,

    // (multiply immediate) stores into register C the result of multiplying register A and value B
    muli,

    // banr (bitwise AND register) stores into register C the result of the bitwise AND of register A and register B
    banr,

    // bani (bitwise AND immediate) stores into register C the result of the bitwise AND of register A and value B
    bani,

    // borr (bitwise OR register) stores into register C the result of the bitwise OR of register A and register B
    borr,

    // bori (bitwise OR immediate) stores into register C the result of the bitwise OR of register A and value B
    bori,

    // setr (set register) copies the contents of register A into register C. (Input B is ignored.)
    setr,

    // seti (set immediate) stores value A into register C. (Input B is ignored.)
    seti,

    // gtir (greater-than immediate/register) sets register C to 1 if value A is greater than register B.
    // Otherwise, register C is set to 0.
    gtir,

    // gtri (greater-than register/immediate) sets register C to 1 if register A is greater than value B. Otherwise,
    // register C is set to 0.
    gtri,

    // gtrr (greater-than register/register) sets register C to 1 if register A is greater than register B. Otherwise,
    // register C is set to 0.
    gtrr,

    // eqir (equal immediate/register) sets register C to 1 if value A is equal to register B. Otherwise, register
    // C is set to 0.
    eqir,

    // eqri (equal register/immediate) sets register C to 1 if register A is equal to value B. Otherwise, register
    // C is set to 0.
    eqri,

    // eqrr (equal register/register) sets register C to 1 if register A is equal to register B. Otherwise, register
    // C is set to 0.
    eqrr,

    nop
};


static constexpr int getNumOfOpCodes()
{
    return static_cast<int>(OpCode::nop);
}

using Registers = std::array<int, 4>;
using Instruction = std::array<int, 4>;


class Machine
{
public:
    Registers registers;

    Machine() :
        registers({0, 0, 0, 0})
    {
    }

    void run(OpCode opcode, int a, int b, int output)
    {
        switch(opcode)
        {
            case OpCode::addr:
                addr(a, b, output);
                break;
            case OpCode::addi:
                addi(a, b, output);
                break;
            case OpCode::mulr:
                mulr(a, b, output);
                break;
            case OpCode::muli:
                muli(a, b, output);
                break;
            case OpCode::banr:
                banr(a, b, output);
                break;
            case OpCode::bani:
                bani(a, b, output);
                break;
            case OpCode::borr:
                borr(a, b, output);
                break;
            case OpCode::bori:
                bori(a, b, output);
                break;
            case OpCode::setr:
                setr(a, b, output);
                break;
            case OpCode::seti:
                seti(a, b, output);
                break;
            case OpCode::gtir:
                gtir(a, b, output);
                break;
            case OpCode::gtri:
                gtri(a, b, output);
                break;
            case OpCode::gtrr:
                gtrr(a, b, output);
                break;
            case OpCode::eqir:
                eqir(a, b, output);
                break;
            case OpCode::eqri:
                eqri(a, b, output);
                break;
            case OpCode::eqrr:
                eqrr(a, b, output);
                break;
            default:
                break;
        }
    }


private:
    void addr(int a, int b, int output)
    {
        registers[output] = registers[a] + registers[b];
    }

    void addi(int a, int b, int output)
    {
        registers[output] = registers[a] + b;
    }

    void mulr(int a, int b, int output)
    {
        registers[output] = registers[a] * registers[b];
    }

    void muli(int a, int b, int output)
    {
        registers[output] = registers[a] * b;
    }

    void banr(int a, int b, int output)
    {
        registers[output] = registers[a] & registers[b];
    }

    void bani(int a, int b, int output)
    {
        registers[output] = registers[a] & b;
    }

    void borr(int a, int b, int output)
    {
        registers[output] = registers[a] | registers[b];
    }

    void bori(int a, int b, int output)
    {
        registers[output] = registers[a] | b;
    }

    void setr(int a, int, int output)
    {
        registers[output] = registers[a];
    }

    void seti(int a, int, int output)
    {
        registers[output] = a;
    }

    void gtir(int a, int b, int output)
    {
        registers[output] = a > registers[b] ? 1 : 0;
    }

    void gtri(int a, int b, int output)
    {
        registers[output] = registers[a] > b ? 1 : 0;
    }

    void gtrr(int a, int b, int output)
    {
        registers[output] = registers[a] > registers[b] ? 1 : 0;
    }

    void eqir(int a, int b, int output)
    {
        registers[output] = a == registers[b] ? 1 : 0;
    }

    void eqri(int a, int b, int output)
    {
        registers[output] = registers[a] == b ? 1 : 0;
    }

    void eqrr(int a, int b, int output)
    {
        registers[output] = registers[a] == registers[b] ? 1 : 0;
    }
};


class Sample
{
public:
    Sample()
    {
        reset();
    }

    void reset()
    {
        registersBefore.fill(0);
        instruction.fill(0);
        registersAfter.fill(0);
    }

    Registers registersBefore;
    Instruction instruction;
    Registers registersAfter;
};


class SampleBuilder
{
public:
    enum class SampleParts
    {
        initialRegisters,
        instruction,
        finalRegisters
    };

    SampleBuilder()
    {
        reset();
    }

    void add(const std::string& line)
    {
        switch(state)
        {
            case SampleParts::initialRegisters:
                readInitialRegisters(line);
                break;

            case SampleParts::instruction:
                readInstruction(line);
                break;

            case SampleParts::finalRegisters:
                readFinalRegisters(line);
                break;

            default:
                break;
        }
    }

    bool isReady() const
    {
        return ready;
    }

    Sample getCurrent() const
    {
        return current;
    }


private:
    bool ready;
    SampleParts state;
    Sample current;

    void reset()
    {
        state = SampleParts::initialRegisters;
        ready = false;
        current.reset();
    }

    void parseRegisters(const std::string& line, Registers& to)
    {
        std::string str = line.substr(line.find('['));
        std::stringstream ss(str);
        for (auto& reg : to)
        {
            ss.ignore(); // skip [ and ,
            ss >> reg;
        }
    }

    void readInitialRegisters(const std::string& line)
    {
        if (line.rfind("Before:") == 0)
        {
            parseRegisters(line, current.registersBefore);
            state = SampleParts::instruction;
        }
        else
        {
            reset();
        }
    }

    void readInstruction(const std::string& line)
    {
        std::stringstream ss(line);
        for (size_t i = 0; i < current.instruction.size(); ++i)
        {
            ss >> current.instruction[i];
        }
        state = SampleParts::finalRegisters;
    }

    void readFinalRegisters(const std::string& line)
    {
        if (line.rfind("After:") == 0)
        {
            parseRegisters(line, current.registersAfter);
            state = SampleParts::initialRegisters;
            ready = true;
        }
        else
        {
            reset();
        }
    }
};


class OpCodeMatcher
{
public:
    static std::vector<OpCode> getMachingOpCodes(const Sample& sample)
    {
        std::vector<OpCode> ops;

        for (int i = 0; i < getNumOfOpCodes(); ++i)
        {
            Machine machine;
            machine.registers = sample.registersBefore;
            machine.run(static_cast<OpCode>(i), sample.instruction[1], sample.instruction[2], sample.instruction[3]);
            if (machine.registers == sample.registersAfter)
            {
                ops.push_back(static_cast<OpCode>(i));
            }
        }

        return ops;
    }
};


class Day16 : public ::testing::Test
{
protected:
    Machine machine;
};


TEST_F(Day16, addr)
{
    machine.registers = {1, 2, 0, 0};
    machine.run(OpCode::addr, 0, 1, 2);
    ASSERT_THAT(machine.registers, testing::ElementsAre(1, 2, 3, 0));
}


TEST_F(Day16, addi)
{
    machine.registers = {1, 0, 0, 0};
    machine.run(OpCode::addi, 0, 3, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(1, 0, 0, 4));
}


TEST_F(Day16, mulr)
{
    machine.registers = {2, 4, 0, 0};
    machine.run(OpCode::mulr, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 4, 0, 8));
}


TEST_F(Day16, muli)
{
    machine.registers = {2, 0, 0, 0};
    machine.run(OpCode::muli, 0, 4, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 8));
}


TEST_F(Day16, banr)
{
    machine.registers = {0, 0, 0xA, 0xF};
    machine.run(OpCode::banr, 2, 3, 0);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0xA, 0, 0xA, 0xF));
}


TEST_F(Day16, bani)
{
    machine.registers = {0, 0, 0xA, 0};
    machine.run(OpCode::bani, 2, 0xF, 1);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 0xA, 0xA, 0));
}


TEST_F(Day16, borr)
{
    machine.registers = {0x7, 0xC, 0, 0};
    machine.run(OpCode::borr, 0, 1, 2);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0x7, 0xC, 0xF, 0));
}


TEST_F(Day16, bori)
{
    machine.registers = {0x7, 0, 0, 0};
    machine.run(OpCode::bori, 0, 0xC, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0x7, 0, 0, 0xF));
}


TEST_F(Day16, setr)
{
    machine.registers = {0, 2, 0, 0};
    machine.run(OpCode::setr, 1, 0, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 2, 0, 2));
}


TEST_F(Day16, seti)
{
    machine.registers = {0, 2, 0, 0};
    machine.run(OpCode::seti, 1, 2, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 2, 0, 1));
}


TEST_F(Day16, gtir)
{
    machine.registers = {2, 0, 0, 0};
    machine.run(OpCode::gtir, 1, 0, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 0));

    machine.run(OpCode::gtir, 2, 0, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 0));

    machine.run(OpCode::gtir, 3, 0, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 1));
}


TEST_F(Day16, gtri)
{
    machine.registers = {2, 0, 0, 0};
    machine.run(OpCode::gtri, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 1));

    machine.run(OpCode::gtri, 0, 2, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 0));

    machine.run(OpCode::gtri, 0, 3, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 0, 0, 0));
}


TEST_F(Day16, gtrr)
{
    machine.registers = {2, 1, 0, 0};
    machine.run(OpCode::gtrr, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 1, 0, 1));

    machine.registers = {2, 2, 0, 0};
    machine.run(OpCode::gtrr, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 2, 0, 0));

    machine.registers = {2, 3, 0, 0};
    machine.run(OpCode::gtrr, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(2, 3, 0, 0));
}


TEST_F(Day16, eqir)
{
    machine.registers = {0, 5, 0, 0};
    machine.run(OpCode::eqir, 5, 1, 2);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 5, 1, 0));

    machine.registers = {0, 5, 0, 0};
    machine.run(OpCode::eqir, 6, 1, 2);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 5, 0, 0));
}


TEST_F(Day16, eqri)
{
    machine.registers = {0, 5, 0, 0};
    machine.run(OpCode::eqri, 1, 5, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 5, 0, 1));

    machine.registers = {0, 5, 0, 0};
    machine.run(OpCode::eqri, 1, 6, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(0, 5, 0, 0));
}


TEST_F(Day16, eqrr)
{
    machine.registers = {5, 5, 0, 0};
    machine.run(OpCode::eqrr, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(5, 5, 0, 1));

    machine.registers = {6, 5, 0, 0};
    machine.run(OpCode::eqrr, 0, 1, 3);
    ASSERT_THAT(machine.registers, testing::ElementsAre(6, 5, 0, 0));
}


TEST_F(Day16, example)
{
    Sample sample;
    sample.registersBefore = {3, 2, 1, 1};
    sample.instruction = {0, 2, 1, 2};
    sample.registersAfter = {3, 2, 2, 1};

    ASSERT_THAT(OpCodeMatcher::getMachingOpCodes(sample), testing::ElementsAre(
        OpCode::addi, OpCode::mulr, OpCode::seti));
}


static void parse(const std::filesystem::path& filepath, std::vector<Sample>& samples,
    std::vector<Registers>& instructions)
{
    try
    {
        std::ifstream input(filepath);
        EXPECT_TRUE(input.is_open());
        std::string line;
        bool readSamples = true;
        int emptyLines = 0;
        SampleBuilder builder;

        while (std::getline(input, line))
        {
            if (readSamples)
            {
                if (line.size() == 0)
                {
                    emptyLines++;
                    if (emptyLines > 2)
                    {
                        readSamples = false;
                        continue;
                    }
                }
                else
                {
                    emptyLines = 0;
                }

                builder.add(line);

                if (builder.isReady())
                {
                    samples.push_back(builder.getCurrent());
                }
            }
            else
            {
                std::stringstream ss(line);
                Instruction instruction;
                for (auto& inst : instruction)
                {
                    ss >> inst;
                }
                instructions.push_back(instruction);
            }
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


static std::array<OpCode, getNumOfOpCodes()> buildOpCodeTable(std::map<int, std::set<OpCode>>& candidates)
{
    std::array<OpCode, getNumOfOpCodes()> table;
    table.fill(OpCode::nop);

    std::set<OpCode> toBeAssigned;
    for (int i = 0; i < getNumOfOpCodes(); ++i)
    {
        toBeAssigned.insert(static_cast<OpCode>(i));
    }

    auto it = candidates.begin();
    while(candidates.size() > 0)
    {
        if (it == candidates.end())
        {
            it = candidates.begin();
        }
        if (it->second.size() == 0)
        {
            it = candidates.erase(it);
            continue;
        }
        if (it->second.size() == 1)
        {
            OpCode op = *it->second.begin();
            assert(table[it->first] == OpCode::nop);
            table[it->first] = op;
            toBeAssigned.erase(op);

            for (auto& c : candidates)
            {
                c.second.erase(op);
            }
        }
        ++it;
    }

    if (toBeAssigned.size() == 1)
    {
        // No candidates left. If only one opcode mapping is missing,
        // it means that the input samples were not complete but we
        // can recover from this situation.
        for (size_t i = 0; i < table.size(); ++i)
        {
            if (table[i] == OpCode::nop)
            {
                table[i] = *toBeAssigned.begin();
            }
        }
    }
    else if (toBeAssigned.size() > 1)
    {
        throw std::runtime_error("Incomplete input. Cannot determine OpCode mapping");
    }

    return table;
}


TEST_F(Day16, solution)
{
    std::vector<Sample> samples;
    std::vector<Instruction> instructions;
    int partOneAnswer = 0;
    std::map<int, std::set<OpCode>> candidates;

    parse(puzzleInputs::getInputDirectory() / "day16_input.txt", samples, instructions);

    for (const auto& sample : samples)
    {
        auto matches = OpCodeMatcher::getMachingOpCodes(sample);
        if (matches.size() >= 3)
        {
            partOneAnswer++;
        }

        for (const auto& op : matches)
        {
            candidates[sample.instruction[0]].insert(op);
        }
    }

    ASSERT_EQ(590, partOneAnswer);

    auto ops = buildOpCodeTable(candidates);
    machine.registers.fill(0);
    for (const auto& inst : instructions)
    {
        machine.run(ops[inst[0]], inst[1], inst[2], inst[3]);
    }
    ASSERT_EQ(475, machine.registers[0]);
}

}
