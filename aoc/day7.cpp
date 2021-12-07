#include "pch.h"
#include "harness.h"


int day7(const string& input)
{
    auto splits = split(input, ",");
    vector<int16_t> nums;
    nums.reserve(splits.size());
    ranges::transform(splits, back_inserter(nums), [](const string& s) { return int16_t(stoi(s)); });

    int max = ranges::max(nums);

    int mincostpos = -1;
    int64_t mincost = INT64_MAX;
    for (int i = 0; i <= max; ++i)
    {
        int64_t cost = 0;
        for (auto num : nums)
        {
            cost += abs(num - i);
        }
        if (cost < mincost)
        {
            mincost = cost;
            mincostpos = i;
        }
    }

    return (int)mincost;
}

int day7_2(const string& input)
{
    auto splits = split(input, ",");
    vector<int16_t> nums;
    nums.reserve(splits.size());
    ranges::transform(splits, back_inserter(nums), [](const string& s) { return int16_t(stoi(s)); });

    int max = ranges::max(nums);

    int mincostpos = -1;
    int mincost = INT_MAX;
    for (int i = 0; i <= max; ++i)
    {
        int cost = 0;
        for (auto num : nums)
        {
            double distance = abs(num - i);
            int fuel = int(distance * (0.5 * (distance + 1)));  // sum of triangular numbers
            cost += fuel;
        }
        if (cost < mincost)
        {
            mincost = cost;
            mincostpos = i;
        }
    }

    return mincost;
}


void run_day7()
{
    string sample = R"(16,1,2,0,4,2,7,1,2,14)";

    test(37, day7(sample));
    gogogo(day7(LOADSTR(7)));

    test(168, day7_2(sample));
    gogogo(day7_2(LOADSTR(7)));
}
