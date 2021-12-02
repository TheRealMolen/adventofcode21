#include "pch.h"
#include "harness.h"


int64_t day2(const stringlist& input)
{
    int64_t depth = 0;
    int64_t horiz = 0;
    for (auto& line : input)
    {
        auto numstart = line.find_last_of(' ') + 1;
        int num = atoi(line.c_str() + numstart);
        switch (line[0])
        {
        case 'f':
            horiz += num;
            break;
        case 'd':
            depth += num;
            break;
        case 'u':
            depth -= num;
            break;
        default:
            throw "nope";
        }
    }

    return (depth * horiz);
}

int64_t day2_2(const stringlist& input)
{
    int64_t depth = 0;
    int64_t horiz = 0;
    int64_t aim = 0;
    for (auto& line : input)
    {
        auto numstart = line.find_last_of(' ') + 1;
        int num = atoi(line.c_str() + numstart);
        switch (line[0])
        {
        case 'f':
            horiz += num;
            depth += aim * num;
            break;
        case 'd':
            aim += num;
            break;
        case 'u':
            aim -= num;
            break;
        default:
            throw "nope";
        }
    }

    return (depth * horiz);
}


void run_day2()
{
    string sample =
R"(forward 5
down 5
forward 8
up 3
down 8
forward 2)";

    test(150, day2(READ(sample)));
    gogogo(day2(LOAD(2)));

    test(900, day2_2(READ(sample)));
    gogogo(day2_2(LOAD(2)));
}