#include "pch.h"
#include "harness.h"
#include <ranges>

static const int kSegCount = 7;


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

string kSegments[] =
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

int descramble(const string& line)
{
    auto parts = split(line, " | ");
    const string& signal = parts[0];
    const string& output = parts[1];

    auto scrambledDigits = split(signal, " ");
    for (string& scrambled : scrambledDigits)
        sort(begin(scrambled), end(scrambled));

    array<vector<string>, kSegCount+1> bucketByLen;
    for (const string& scrambled : scrambledDigits)
    {
        size_t len = scrambled.length();
        auto& bucket = bucketByLen[len];
        if (ranges::find(bucket, scrambled) == end(bucket))
            bucket.push_back(scrambled);
    }

    // now start deducing...

    // we need a 1, 4 and 7 for our super smart heuristical analysis to work...
    _ASSERT(!bucketByLen[2].empty() && !bucketByLen[3].empty() && !bucketByLen[4].empty());
    _ASSERT(bucketByLen[6].size() == 3);

    const string& oneScrambled = bucketByLen[2].front();
    const string& sevenScrambled = bucketByLen[3].front();
    char topSegment = sevenScrambled[sevenScrambled.find_first_not_of(oneScrambled)];

    // now we can use 4 to find a couple more -- remove the "1" segs, and the remainder are either TL & Mid
    string& fourScrambled = bucketByLen[4].front();
    fourScrambled.erase(remove_if(begin(fourScrambled), end(fourScrambled), [&](char c) { return oneScrambled.find(c) != string::npos; }), end(fourScrambled));

    // then we deal with 6, 9 and 0
    auto& sixZeroNineScrambled = bucketByLen[6];
    // we can distinguish between 6 & 9/0 because 6 won't be a superset of 1 and the others will
    ranges::partition(sixZeroNineScrambled, [&](const string& s) { return !includes(begin(s), end(s), begin(oneScrambled), end(oneScrambled)); });
    // likewise, of 9 and 0, only 9 will be a superset of 4
    partition(begin(sixZeroNineScrambled) + 1, end(sixZeroNineScrambled), [&](const string& s) { return !includes(begin(s), end(s), begin(fourScrambled), end(fourScrambled)); });
    const string& sixScrambled = sixZeroNineScrambled[0];
    const string& zeroScrambled = sixZeroNineScrambled[1];
    const string& nineScrambled = sixZeroNineScrambled[2];

    // the bottom left is in 6 but not 9
    char botLeftSegment = sixScrambled[sixScrambled.find_first_not_of(nineScrambled)];
    // the middle is in 6 but not in 0
    char middleSegment = sixScrambled[sixScrambled.find_first_not_of(zeroScrambled)];
    // the top right is in 1 but not 6
    char topRightSegment = oneScrambled[oneScrambled.find_first_not_of(sixScrambled)];
    // the bottom right is in 1 but isn't the top right
    char botRightSegment = oneScrambled[oneScrambled.find_first_not_of(topRightSegment)];

    // the top left is fiddly
    string notTopLeftScrambled{ middleSegment, topRightSegment, botRightSegment };
    char topLeftSegment = fourScrambled[fourScrambled.find_first_not_of(notTopLeftScrambled)];
    // the bottom segment is a bit more fiddly!
    string notBotScrambled{ topSegment, topLeftSegment, topRightSegment, botLeftSegment, botRightSegment };
    char bottomSegment = zeroScrambled[zeroScrambled.find_first_not_of(notBotScrambled)];

    // now build a little remap table...
    char remap[256];
    ranges::fill(remap, '!');
    remap[topSegment]       = 'a';
    remap[topLeftSegment]   = 'b';
    remap[topRightSegment]  = 'c';
    remap[middleSegment]    = 'd';
    remap[botLeftSegment]   = 'e';
    remap[botRightSegment]  = 'f';
    remap[bottomSegment]    = 'g';

    int display = 0;
    for (const string& scrambled : split(output, " "))
    {
        display *= 10;

        string unscrambled;
        ranges::transform(scrambled, back_inserter(unscrambled), [&](char c) { return remap[c]; });
        _ASSERT(unscrambled.find('!') == string::npos);
        ranges::sort(unscrambled);

        size_t digit = ranges::find(kSegments, unscrambled) - &kSegments[0];
        display += (int)digit;
    }

    return display;
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

    test(5353, descramble("acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf"));
    gogogo(day8_2(LOAD(8)));
}