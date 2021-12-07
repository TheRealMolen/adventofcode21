#include "pch.h"
#include "harness.h"

#include <smmintrin.h>
#include <immintrin.h>


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

int day7_2_original(const string& input)
{
    ScopeTimer full("full_og");
    auto splits = split(input, ",");
    vector<int16_t> crabs;
    crabs.reserve(splits.size());
    ranges::transform(splits, back_inserter(crabs), [](const string& s) { return int16_t(stoi(s)); });

    ScopeTimer algo("algo_og");

    int max = ranges::max(crabs);

    int mincostpos = -1;
    int mincost = INT_MAX;
    for (int i = 0; i <= max; ++i)
    {
        int cost = 0;
        for (auto crab : crabs)
        {
            int distance = abs(crab - i);
            int fuel = (distance * (distance + 1)) >> 1;  // sum of triangular numbers
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

int day7_2_avx(const string& input)
{
    //ScopeTimer full("full_avx");
    auto splits = split(input, ",");
    int numCrabs = (int)splits.size();

    static constexpr int alignment = 64;
    size_t alloc_size = (sizeof(int16_t) * numCrabs + (alignment - 1)) & ~(alignment - 1);
    int16_t* crabs = reinterpret_cast<int16_t*>(_aligned_malloc(alloc_size, alignment));
    memset(crabs, 0, alloc_size);

    ranges::transform(splits, crabs, [](const string& s) { return int16_t(stoi(s)); });

    //ScopeTimer algo("algo_avx");

    int max = *max_element(crabs, crabs + numCrabs);

    const int crabsPerLoop = 16;    // 16 int16s in an mm256
    const int numFancyCrabs = numCrabs & ~(crabsPerLoop - 1);
    const int16_t* endFancyCrabs = crabs + numFancyCrabs;
    const int numRegularCrabs = numCrabs - numFancyCrabs;

    using i16vec = __m256i;
    using i32vec = __m256i;

    const i16vec one16 = _mm256_set1_epi16(1);

    int mincostpos = -1;
    int mincost = INT_MAX;
    for (int i = 0; i <= max; ++i)
    {
        i32vec dcost8 = _mm256_setzero_si256();
        i16vec pos16 = _mm256_set1_epi16(int16_t(i));
        for (int16_t* crab = crabs; crab != endFancyCrabs; crab += crabsPerLoop)
        {
            i16vec crabs16 = _mm256_load_si256((__m256i*)crab);

            i16vec delta16 = _mm256_sub_epi16(crabs16, pos16);
            i16vec dist16 = _mm256_abs_epi16(delta16);

            // int fuel = (distance * (distance + 1)) >> 1;
            i16vec distplusone16 = _mm256_add_epi16(dist16, one16);     // (distance + 1)

            // now to mul by distance, we do it twice to get the high and low 16 bits of the result, and then combine
            i16vec dfuel16_lobits = _mm256_mullo_epi16(dist16, distplusone16);
            i16vec dfuel16_hibits = _mm256_mulhi_epi16(dist16, distplusone16);
            i32vec dfuel8_lo = _mm256_unpacklo_epi16(dfuel16_lobits, dfuel16_hibits);
            i32vec dfuel8_hi = _mm256_unpackhi_epi16(dfuel16_lobits, dfuel16_hibits);

            // add both of those fuel usages onto our running total
            dcost8 = _mm256_add_epi32(dcost8, dfuel8_lo);
            dcost8 = _mm256_add_epi32(dcost8, dfuel8_hi);
        }

        // note that dcost8 is double the fuel cost -- so now we halve
        i32vec cost8 = _mm256_srai_epi32(dcost8, 1);

        // sum the 8 values in cost8
        i32vec cost4 = _mm256_hadd_epi32(cost8, cost8);
        i32vec cost2 = _mm256_hadd_epi32(cost4, cost4);
        int cost = _mm256_extract_epi32(cost2, 0) + _mm256_extract_epi32(cost2, 4);

        // now pick up the last <16
        for (int c = 0; c < numRegularCrabs; ++c)
        {
            int crab = endFancyCrabs[c];
            int distance = abs(crab - i);
            int fuel = (distance * (distance + 1)) >> 1;
            cost += fuel;
        }

        if (cost < mincost)
        {
            mincost = cost;
            mincostpos = i;
        }
    }

    _aligned_free(crabs);

    return mincost;
}


void run_day7()
{
    string sample = R"(16,1,2,0,4,2,7,1,2,14)";

    test(37, day7(sample));
    gogogo(day7(LOADSTR(7)));

    test(168, day7_2_avx(sample));
    //gogogo(day7_2_original(LOADSTR(7)), 95476244);
    gogogo(day7_2_avx(LOADSTR(7)), 95476244);
}
