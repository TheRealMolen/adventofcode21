#include "pch.h"
#include "harness.h"
#include "pt3.h"


using Pt = Pt3i;


struct Report
{
    int scannernum = -1;
    Pt pos = {0,0,0};
    vector<Pt> points;

    Report(const Report&) = default;
    Report(int num = -1) : scannernum(num)
    {
        points.reserve(30);
    }
    Report(Report&& moveFrom) noexcept : points(move(moveFrom.points))
    {
        scannernum = moveFrom.scannernum;
        pos = moveFrom.pos;
    }
    Report& operator=(const Report&) = default;
};

auto begin(Report& r) { return begin(r.points); }
auto begin(const Report& r) { return begin(r.points); }
auto end(Report& r) { return end(r.points); }
auto end(const Report& r) { return end(r.points); }

using Reports = vector<Report>;

inline Pt spinAroundX(const Pt& p, int spin)
{
    switch (spin)
    {
    case 0: return p;
    case 1: return { p.x, -p.z,  p.y };
    case 2: return { p.x, -p.y, -p.z };
    case 3: return { p.x,  p.z, -p.y };
    }
    throw "wtaf";
}
inline Pt spinAroundY(const Pt& p, int spin)
{
    switch (spin)
    {
    case 0: return p;
    case 1: return { -p.z, p.y,  p.x };
    case 2: return { -p.x, p.y, -p.z };
    case 3: return {  p.z, p.y, -p.x };
    }
    throw "wtaf";
}
inline Pt spinAroundZ(const Pt& p, int spin)
{
    switch (spin)
    {
    case 0: return p;
    case 1: return { -p.y,  p.x, p.z };
    case 2: return { -p.x, -p.y, p.z };
    case 3: return {  p.y, -p.x, p.z };
    }
    throw "wtaf";
}

// assuming initial axis is +x and initial spin is 0
// rotates so that +x moves to _axis_ and then rotates _spin_*90deg around that axis
// axis is: +x, +y, -x, -y, +z, -z
inline Pt rotate(const Pt& p, int axis, int spin)
{
    switch(axis)
    {
    case 0: return spinAroundX(p, spin);
    case 1: return spinAroundY(spinAroundZ(p, 1), spin);
    case 2: return spinAroundX(spinAroundZ(p, 2), spin);
    case 3: return spinAroundY(spinAroundZ(p, 3), spin);
    case 4: return spinAroundZ(spinAroundY(p, 1), spin);
    case 5: return spinAroundZ(spinAroundY(p, 3), spin);
    }

    throw "good grief";
}

void testRotate()
{
    Pt p {1,2,3};
    set<Pt> rots;
    for (int axis = 0; axis < 6; ++axis)
    {
        for (int spin = 0; spin < 4; ++spin)
        {
            //cout << "rot " << p << " by " << axis << "/" << spin << " => " << rotate(p, axis, spin) << endl;
            auto itInserted = rots.insert(rotate(p, axis, spin));
            if (!itInserted.second)
                cout << RED << "dup rotate: " << RESET << axis << "/" << spin << " resulted in " << rotate(p, axis, spin) << endl;
        }
    }
    test(24, rots.size());

    int mismatches = 0;
    for (auto& r : rots)
    {
        if (r.magSq() != p.magSq())
            ++mismatches;
    }
    test(0, mismatches);
}

Reports parseScannerReports(const stringlist& input)
{
    Reports reports;
    Report curr;
    for (auto& line : input)
    {
        if (line.length() == 0)
            continue;

        if (line[1] == '-')   // --- scanner 3 ---
        {
            if (!curr.points.empty())
            {
                reports.emplace_back(move(curr));
                curr = {};
            }
            curr.scannernum = atoi(line.c_str() + 12);
        }
        else
        {
            curr.points.emplace_back(line);
        }
    }
    reports.emplace_back(move(curr));

    return reports;
}

uint32_t MinOverlaps = 12;

#define D_ENABLE_LOGx
#ifdef D_ENABLE_LOG
ostream* g_log = nullptr;
#define LOG(expr)   do { if (g_log) { *g_log << expr << '\n'; } } while(0)
#else
#define LOG(expr)   do{}while(0)
#endif

bool checkTranslation(const Pt3i& refToTest, const Pt3i* pTest, const Pt3i* testBufEnd, const Pt3i* pRef, const Pt3i* refBufEnd, Report& working)
{
    uint32_t remaining = MinOverlaps - 1;   // -1 because every test point is an overlap

    for (auto itTest = pTest + 1; itTest <= (testBufEnd - remaining); ++itTest)
    {
        LOG("     .. testing next point " << *itTest << "....");
        for (auto itRef = pRef + 1; itRef != refBufEnd; ++itRef)
        {
            LOG("      .. does it match " << *itRef << "? xlation gives " << (*itRef + refToTest));
            if (*itRef + refToTest == *itTest)
            {
                LOG("      .. YES!");
                --remaining;
                if (remaining == 0)
                {
                    for (auto& p : working)
                        p -= refToTest;

                    working.pos -= refToTest;

                    return true;
                }
                break;
            }
        }
    }

    return false;
}

bool checkForOverlap(const Report& ref, const Report& test, Report& working)
{
    auto refBuf = ref.points.data();
    auto refBufEnd = refBuf + ref.points.size();

    for (int axis = 0; axis < 6; ++axis)
    {
        for (int spin = 0; spin < 4; ++spin)
        {
            LOG(" .. checking " << axis << "/" << spin);
            working.points.clear();

            ranges::transform(test, back_inserter(working.points), [=](const Pt3i& p) { return rotate(p, axis, spin); });
            ranges::sort(working);
            LOG(" .. first point @ " << axis << "/" << spin << ": " << working.points.front());

            auto testBuf = working.points.data();
            auto testBufEnd = testBuf + working.points.size();
            for (auto pTest = testBuf; pTest != testBufEnd; ++pTest)
            {
                for (auto pRef = refBuf; pRef != refBufEnd; ++pRef)
                {
                    LOG("   .. assuming that " << *pTest << " matches " << *pRef << " -- xlate " << refToTest << " -- scanner @ " << (rotate(working.pos, axis, spin) - refToTest));
                    Pt3i refToTest = *pTest - *pRef;
                    if (checkTranslation(refToTest, pTest, testBufEnd, pRef, refBufEnd, working))
                    {
                        //cout << "    .. scanner " << working.scannernum << " @ " << axis << "/" << spin << " overlaps " << ref.scannernum << endl;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

// on return, _working_ has the report in the reference frame
unordered_set<uint16_t> g_alreadyTested;
Reports::iterator findOneMatch(const Reports& inRefFrame, Reports& reports, Report& working)
{
    working.points.reserve(30);

    for (auto& ref : inRefFrame)
    {
        for (auto it = begin(reports); it != end(reports); ++it)
        {
            working = *it;

            uint16_t key = uint16_t(ref.scannernum + working.scannernum * 100);
            if (g_alreadyTested.contains(key))
                continue;
            g_alreadyTested.insert(key);

            //cout << " -- checking scanner " << working.scannernum << " against ref " << ref.scannernum << endl;
            if (checkForOverlap(ref, *it, working))
            {
                //cout << "  ..found overlap between scanners " << ref.scannernum << " & " << working.scannernum << ". Scanner " << working.scannernum << " pos in ref frame: " << working.pos << endl;
                return it;
            }
        }
    }

    return end(reports);
}

int maxDistBetweenScanners = -1;

int day19(const stringlist& input)
{
    TIME_SCOPE(d19);

#ifdef D_ENABLE_LOG
    ofstream logfs(format("day19_{}.txt", input.size()));
    g_log = &logfs;
#endif

    Reports reports = parseScannerReports(input);

    // use report 0 as reference frame
    Reports inRefFrame;
    inRefFrame.emplace_back(reports.front());
    ranges::sort(inRefFrame.back());
    reports.erase(begin(reports));

    set<Pt> allKnown(begin(inRefFrame.front()), end(inRefFrame.front()));

    g_alreadyTested.clear();
    Report working;
    while (!reports.empty())
    {
        auto foundIt = findOneMatch(inRefFrame, reports, working);
        if (foundIt == end(reports))
        {
            cout << RED << "failed to find match. known verts are:\n" << RESET;
            for (const auto& p : allKnown)
                cout << p << "\n";
            cout << endl;

            return -1;
        }

        auto& newReport = inRefFrame.emplace_back(move(working));

        ranges::sort(newReport);
        ranges::copy(newReport, inserter(allKnown, begin(allKnown)));

        reports.erase(foundIt);
    }

#ifdef D_ENABLE_LOG
    g_log = nullptr;
#endif

    const Report* a = &inRefFrame.front();
    const Report* b = a;
    for (auto it = begin(inRefFrame); next(it) != end(inRefFrame); ++it)
    {
        for (auto itN = next(it); itN != end(inRefFrame); ++itN)
        {
            auto diff = it->pos - itN->pos;
            int dist = abs(diff.x) + abs(diff.y) + abs(diff.z);
            if (maxDistBetweenScanners < dist)
            {
                maxDistBetweenScanners = dist;
                a = &*it;
                b = &*itN;
            }
        }
    }
    cout << "** max manhattan dist between scanners " << a->scannernum << " & " << b->scannernum << ": " << maxDistBetweenScanners << endl;

    return int(allKnown.size());
}


void run_day19()
{
    string sample =
R"(--- scanner 0 ---
-1,-1,1
-2,-2,2
-3,-3,3
-2,-3,1
5,6,-4
8,0,7

--- scanner 1 ---
-3,-3,3
11,-1,1
12,-2,2
12,-1,3
5,-6,4,
8,0,-7)";

    testRotate();

    MinOverlaps = 3;
    test(9, day19(READ(sample)));
    MinOverlaps = 12;

    auto testInput2 = LOAD(19t);
    testInput2.erase(begin(testInput2) + 54, end(testInput2));
    test(38, day19(testInput2));

    test(79, day19(LOAD(19t)));
    test(3621, maxDistBetweenScanners);

    nonono(day19(LOAD(19)));

    nonono(maxDistBetweenScanners);
}