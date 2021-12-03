#include "pch.h"
#include "harness.h"


int day3(const stringlist& input)
{
    int bitlength = (int)input[0].length();
    vector<int> bitcounts(bitlength, 0);

    for (auto& line : input)
    {
        for (int bit = 0; bit < bitlength; ++bit)
        {
            if (line[bit] == '1')
                ++bitcounts[bit];
        }
    }

    int threshold = (int)((input.size() + 1) / 2);
    int gamma = 0;
    for (int bitcount : bitcounts)
    {
        gamma = (gamma << 1);
        if (bitcount >= threshold)
            gamma |= 1;
    }

    int epsilon = ((1 << bitlength) - 1) ^ gamma;

    return gamma * epsilon;
}

template<typename KeepFunc>
int find_rating(const stringlist& input, const KeepFunc& keepfunc)
{
    vector<const char*> remaining;
    remaining.reserve(input.size());
    ranges::transform(input, back_inserter(remaining), [](auto& line) { return line.c_str(); });

    vector<const char*> ones;
    vector<const char*> zeros;
    ones.reserve(input.size());
    zeros.reserve(input.size());
    int currentbit = 0;
    do
    {
        for (auto& line : remaining)
        {
            if (line[currentbit] == '1')
                ones.push_back(line);
            else
                zeros.push_back(line);
        }

        if (keepfunc((int)ones.size(), (int)zeros.size()))
            remaining.swap(ones);
        else
            remaining.swap(zeros);

        ones.clear();
        zeros.clear();
        ++currentbit;

    } while (remaining.size() > 1);

    int rating = 0;
    for (const char* pc = remaining.front(); *pc; ++pc)
    {
        rating = (rating << 1);
        if (*pc == '1')
            rating |= 1;
    }

    return rating;
}

int day3_2(const stringlist& input)
{
    int oxy_rating = find_rating(input, [](int numOnes, int numZeros) { return numOnes >= numZeros; });
    int co2_rating = find_rating(input, [](int numOnes, int numZeros) { return numOnes < numZeros; });
    return oxy_rating * co2_rating;
}


void run_day3()
{
    string sample =
R"(00100
11110
10110
10111
10101
01111
00111
11100
10000
11001
00010
01010)";

    test(198, day3(READ(sample)));
    gogogo(day3(LOAD(3)));

    test(230, day3_2(READ(sample)));
    gogogo(day3_2(LOAD(3)));
}