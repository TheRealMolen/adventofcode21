#include "pch.h"

#include "harness.h"


int day1(const stringlist& input)
{
    int last = INT_MAX;
    int increases = 0;
    for (const string& line : input)
    {
        int depth = atoi(line.c_str());
        if (depth > last)
            increases++;
        last = depth;
    }
    return increases;
}


int64_t day1_2(const stringlist& input)
{
    vector<int> parsed;
    parsed.reserve(input.size());
    ranges::transform(input, back_inserter(parsed), [](const string& line) -> int { return atoi(line.c_str()); });

    int last = INT_MAX;
    int increases = 0;
    for (size_t i = 0; i + 2 < parsed.size(); ++i)
    {
        int total = parsed[i] + parsed[i + 1] + parsed[i + 2];
        if (total > last)
            increases++;
        last = total;
    }
    return increases;
}


void run_day1()
{
    string testInput =
R"(199
200
208
210
200
207
240
269
260
263)";

    test(7, day1(READ(testInput)));
    gogogo(day1(LOAD(1)));

    test(5, day1_2(READ(testInput)));
    gogogo(day1_2(LOAD(1)));
}
