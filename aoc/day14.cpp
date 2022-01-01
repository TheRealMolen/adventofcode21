#include "pch.h"
#include "harness.h"

using PolyKey = uint16_t;
using PolyRules = vector<pair<PolyKey, char>>;
using Polymer = vector<char>;
using PolyPairCount = unordered_map<PolyKey,int64_t>;

pair<Polymer,PolyRules> parsePolymers(const stringlist& input)
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

int64_t day14(const stringlist& input, int numSteps=10)
{
    auto [poly, rules] = parsePolymers(input);
    PolyPairCount count, working;
    for (auto& [key, val] : rules)
    {
        count.try_emplace(key, 0);
        working.try_emplace(key, 0);
    }

    char* in = poly.data();
    const char* end = in + poly.size() - 1;
    for (; in != end; ++in)
    {
        PolyKey key = *reinterpret_cast<const PolyKey*>(in);
        ++count[key];
    }

    for (int i=0; i<numSteps; ++i)
    {
        for (auto& [key, oldval] : count)
        {
            const char* oldpair = reinterpret_cast<const char*>(&key);
            char newel = findElementToInsert(oldpair, rules);

            char newpair[2] = { oldpair[0], newel };
            PolyKey* newkey = reinterpret_cast<PolyKey*>(&newpair[0]);
            working[*newkey] += oldval;

            newpair[0] = newel;
            newpair[1] = oldpair[1];
            working[*newkey] += oldval;
        }

        count.swap(working);
        for (auto& kv : working)
            kv.second = 0;
    }

    int64_t hist[256];
    ranges::fill(hist, 0);
    for (auto& [key, val] : count)
    {
        const char* oldpair = reinterpret_cast<const char*>(&key);
        hist[oldpair[0]] += val;
    }
    hist[poly.back()]++;

    auto maxCount = ranges::max(hist);
    auto minCount = ranges::min(hist | views::filter([](auto count) { return count > 0; }));

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

    test<int64_t>(1588, day14(READ(sample)));
    gogogo(day14(LOAD(14)));

    test<int64_t>(2188189693529, day14(READ(sample), 40));
    gogogo(day14(LOAD(14), 40));
}