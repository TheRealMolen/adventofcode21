#include "pch.h"
#include "harness.h"

struct Cave
{
    string name;
    vector<Cave*> nearby;
    uint8_t numNearby = 0;
    bool isBig;
    int8_t visitCount = 0;

    Cave(const string& n) : name(n), isBig(isupper(n[0])) { /**/ }
};

using CaveSystem = unordered_map<string, unique_ptr<Cave>>;


CaveSystem loadCaveSystem(const stringlist& input)
{
    CaveSystem caves;
    auto ensureCreated = [&](const string& name)
    {
        auto itCave = caves.find(name);
        if (itCave != caves.end())
            return;

        caves.emplace(name, make_unique<Cave>(name));
    };

    for (auto& line : input)
    {
        auto ends = split(line, "-");
        ensureCreated(ends[0]);
        ensureCreated(ends[1]);

        auto itFrom = caves.find(ends[0]);
        auto itTo = caves.find(ends[1]);

        Cave* from = itFrom->second.get();
        Cave* to = itTo->second.get();

        from->nearby.push_back(to);
        to->nearby.push_back(from);
        from->numNearby++;
        to->numNearby++;
    }

    return caves;
}

using CavePathNode = pair<Cave*, int>;
using CavePath = vector<CavePathNode>;

ostream& operator<<(ostream& os, const CavePath& path)
{
    for (const auto& cavePlusIx : path)
    {
        if (&cavePlusIx != &path.front())
            os << ",";
        os << cavePlusIx.first->name;
    }
    return os;
}

int day12(const stringlist& input, bool allowDoubleVisit = false)
{
    auto caves = loadCaveSystem(input);

    Cave* const start = caves.find("start")->second.get();
    Cave* const end = caves.find("end")->second.get();

    // NB. uglified for Debug performance... (manual tracking of pathLen & numNearby was worth 4x speedup)
    static const int MaxPathLen = 1000;
    CavePath path(MaxPathLen, CavePathNode(nullptr, -1));
    size_t pathLen = 0;

    int totalPaths = 0;
    path[pathLen].first = start;
    path[pathLen].second = -1;
    ++pathLen;
    const Cave* doubleVisitCave = allowDoubleVisit ? nullptr : start;
    start->visitCount = 2;
    while (pathLen > 0)
    {
        // try the next available nearby cave
        auto& currNode = path[pathLen-1];
        Cave* cave = currNode.first;
        int nextCaveIx = currNode.second + 1;
        Cave* nextCave = nullptr;
        while(nextCaveIx < int(cave->numNearby))
        {
            nextCave = cave->nearby[nextCaveIx];
            int maxVisitsAllowed = (doubleVisitCave == nullptr) ? 2 : 1;
            if (nextCave->isBig || nextCave->visitCount < maxVisitsAllowed)
                break;

            nextCave = nullptr;
            ++nextCaveIx;
        }

        if (nextCave)
        {
            path[pathLen - 1].second = nextCaveIx;

            if (!nextCave->isBig)
            {
                ++nextCave->visitCount;
                if (nextCave->visitCount > 1)
                {
                    _ASSERT(!doubleVisitCave);
                    doubleVisitCave = nextCave;
                }
            }
            path[pathLen].first = nextCave;
            path[pathLen].second = -1;
            pathLen++;

            if (nextCave == end)
                totalPaths++;
        }

        if (!nextCave || nextCave == end)
        {
            // none left, so pop this cave off the list
            Cave* lastCave = path[pathLen - 1].first;
            lastCave->visitCount--;
            if (lastCave == doubleVisitCave)
                doubleVisitCave = nullptr;
            --pathLen;
        }
    }

    return totalPaths;
}


void run_day12()
{
    string sample =
R"(start-A
start-b
A-c
A-b
b-d
A-end
b-end)";

    string sample2 =
R"(dc-end
HN-start
start-kj
dc-start
dc-HN
LN-dc
HN-end
kj-sa
kj-HN
kj-dc)";

    string sample3 =
R"(fs-end
he-DX
fs-he
start-DX
pj-DX
end-zg
zg-sl
zg-pj
pj-he
RW-he
fs-DX
pj-RW
zg-RW
start-pj
he-WI
zg-he
pj-fs
start-RW)";

    test(10, day12(READ(sample)));
    test(19, day12(READ(sample2)));
    test(226, day12(READ(sample3)));
    gogogo(day12(LOAD(12)));

    test(36, day12(READ(sample), true));
    test(103, day12(READ(sample2), true));
    test(3509, day12(READ(sample3), true));
    gogogo(day12(LOAD(12), true));
}