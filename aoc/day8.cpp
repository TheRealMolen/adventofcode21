#include "pch.h"
#include "harness.h"
#include <ranges>

inline bool is_unique_digit(const string& digit)
{
    switch (digit.length())
    {
    case 2: // 1
    case 3: // 7
    case 4: // 4
    case 7: // 8
        return true;
    }
    return false;
}

int day8(const stringlist& input)
{
    int numuniques = 0;
    for (auto& line : input)
    {
        string output = split(line, "|")[1];
        trim(output);
        auto digits = split(output, " ");
        numuniques += transform_reduce(begin(digits), end(digits), 0, plus{}, is_unique_digit);
    }

    return numuniques;
}

const char* kSegments[] =
{
    "abcefg",
    "cf",
    "acdeg",
    "acdfg",
    "bcdf",
    "abdfg",
    "abdefg",
    "acf",
    "abcdefg",
    "abcdfg",
};
static const int kNumSegments = 7;

struct DigitInfo
{
    const char* segments;
    int16_t number;
};
using DigitsPerLength = array<vector<DigitInfo>, kNumSegments>; // map from number of ON segments to the list of digits that could be represented

DigitsPerLength g_digitInfo;

void compileDigitInfo()
{
    for (int i=0; i<10; ++i)
    {
        const char* segments = kSegments[i];
        int len = strlen(segments);        
        g_digitInfo[len].push_back({ segments, (int16_t)i });
    }
}

int descramble(const string& line)
{
    auto parts = split(line, " | ");
    const string& signal = parts[0];
    const string& output = parts[1];

    auto digits = split(signal, " ");
    uint16_t possibles[kNumSegments]; // if (possibles[1] & (1<<3)) is set, it means that "b" might map to segment "d"
    ranges::fill(possibles, 0x7f);  // 7f is all 7 segments

    for (const string& scrambled : digits)
    {
        int len = scrambled.length();
        if (len == kNumSegments)
            continue;   // stupid 8 tells us NOTHING

        const auto& potentials = g_digitInfo[len];
        for (auto& digitInfo : potentials)
        {

        }

        //uint8_t mask = 0;
        //const char* curr;
        //for (curr = segments; *curr; ++curr)
        //    mask |= 1 << (*curr - 'a');
    }

    return (int)(signal.size() + output.size());
}

int day8_2(const stringlist& input)
{
    int total = 0;
    for (auto& line : input)
    {
        total += descramble(line);
    }

    return total;
}


void run_day8()
{
    string sample =
R"(be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe
edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc
fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg
fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb
aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea
fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb
dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe
bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef
egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb
gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce)";

    test(26, day8(READ(sample)));
    gogogo(day8(LOAD(8)));
    
    compileDigitInfo();

    test(5353, descramble("acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf"));
    //test(-100, day8_2(READ(sample)));
    //gogogo(day8_2(LOAD(8)));
}