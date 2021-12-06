#include "pch.h"
#include "harness.h"


static constexpr int kTimeBetweenSpawns = 7;
static constexpr int kTimeToFirstSpawn = 2;
static constexpr int kNumBuckets = kTimeBetweenSpawns + kTimeToFirstSpawn;


int64_t day6(const string& input, int numDays=80)
{
    vector<int64_t> buckets(kNumBuckets, 0);

    // put the fish in the buckets
    for (const string& bucket : split(input, ","))
    {
        buckets[stoi(bucket)]++;
    }

    for (int day = 0; day < numDays; ++day)
    {
        int64_t spawners = buckets[0];
        buckets.erase(buckets.begin());
        buckets.push_back(spawners);
        buckets[kTimeBetweenSpawns - 1] += spawners;
    }

    return accumulate(begin(buckets), end(buckets), 0ll);
}


void run_day6()
{
    string sample =
        R"(3,4,3,1,2)";

    test<int64_t>(26, day6(sample, 18));
    test<int64_t>(5934, day6(sample, 80));
    gogogo(day6(LOADSTR(6)));

    test(26984457539ll, day6(sample, 256));
    gogogo(day6(LOADSTR(6), 256));
}