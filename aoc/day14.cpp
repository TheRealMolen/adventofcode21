#include "pch.h"
#include "harness.h"

static const int kNumElements = 24; // there are no characters after V in the data, so 24 letters is enough
using PolyKey = uint16_t;
using PolyRules = vector<pair<PolyKey, char>>;
using Polymer = vector<char>;
using PolyHist = array<int64_t, kNumElements>;

ostream& operator<<(ostream& os, Polymer poly)
{
    os.write(poly.data(), poly.size());
    return os;
}


pair<Polymer,PolyRules> parseInput(const stringlist& input)
{
    PolyRules rules;
    rules.reserve(input.size() - 2);
    for (auto itLine = input.begin() + 2; itLine != input.end(); ++itLine)
    {
        istringstream is(*itLine);
        string keyStr;
        char el;
        is >> keyStr >> " -> " >> el;
        PolyKey key = *reinterpret_cast<const PolyKey*>(keyStr.c_str());
        rules.emplace_back(key, el);
    }

    ranges::sort(rules);

    return {{input[0].begin(), input[0].end()}, rules};
}

char findElementToInsert(const char* old, const PolyRules& rules)
{
    PolyKey key = *reinterpret_cast<const PolyKey*>(old);
    auto itFound = ranges::find_if(rules, [=](const auto& rule) { return key == rule.first; });
    if (itFound != rules.end())
        return itFound->second;

    _ASSERT(false);
    return '!';
}

void tickPoly(Polymer& poly, Polymer& working, const PolyRules& rules)
{
    working.clear();
    char* in = poly.data();
    const char* end = in + poly.size() - 1;
    for ( ; in != end; ++in)
    {
        char newEl = findElementToInsert(in, rules);

        working.push_back(*in);
        working.push_back(newEl);
    }
    working.push_back(*in);

    poly.swap(working);
}

void dumpHist(const Polymer& poly, const PolyRules& rules, bool header = false)
{
    PolyHist hist;
    PolyHist ruleHist;
    ranges::fill(hist, 0);
    ranges::fill(ruleHist, 0);
    for (uint8_t c : poly)
        hist[c - 'A']++;
    for (const auto& rule : rules)
        ruleHist[rule.second - 'A']++;

    if (header)
    {
        for (int c = 0; c < kNumElements; ++c)
        {
            if (ruleHist[c])
                cout << char(c + 'A') << '\t';
        }
        cout << endl;
    }

    for (int c = 0; c < kNumElements; ++c)
    {
        if (!ruleHist[c])
            continue;

        cout << hist[c] << '\t';
    }
    cout << endl;
}

Polymer run_poly(const stringlist& input, int numSteps, ostream* pos=nullptr)
{
    auto [poly, rules] = parseInput(input);
    Polymer working;

    //cout << "0\t" << poly.size() << endl;
    //dumpHist(poly, rules, true);
    for (int i=1; i<=numSteps; ++i)
    {
        tickPoly(poly, working, rules);
        //cout << i << "\t" << poly.size() << endl;
        if (pos)
            (*pos) << poly << endl;
        //dumpHist(poly, rules);
    }

    return poly;
}


int day14(const stringlist& input)
{
    //ofstream ofs(input.front() + ".txt");

    auto poly = run_poly(input, 10/*, &ofs*/);

    int hist[256];
    ranges::fill(hist, 0);
    for (uint8_t c : poly)
        hist[c]++;

    int maxCount = ranges::max(hist);
    int minCount = ranges::min(hist | views::filter([](int count) { return count > 0; }));

    return maxCount - minCount;
}


void run_day14()
{
    string sample =
        R"(NNCB

CH -> B
HH -> N
CB -> H
NH -> C
HB -> C
HC -> B
HN -> C
NN -> C
BH -> H
NC -> B
NB -> B
BN -> B
BB -> N
BC -> B
CC -> N
CN -> C)";

    string tinySample =
R"(HH

CH -> B
HH -> N
CB -> H
NH -> C
HB -> C
HC -> B
HN -> C
NN -> C
BH -> H
NC -> B
NB -> B
BN -> B
BB -> N
BC -> B
CC -> N
CN -> C)";

    test(1588, day14(READ(tinySample)));

    test(7, run_poly(READ(sample), 1).size());
    test(97, run_poly(READ(sample), 5).size());
    test(1588, day14(READ(sample)));
    gogogo(day14(LOAD(14)));

    //test(-100, day14_2(READ(sample)));
    //gogogo(day14_2(LOAD(14)));
    skip("i'm stumped for now");
}