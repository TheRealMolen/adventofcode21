#include "pch.h"
#include "harness.h"
#include "pt2.h"

#include <execution>


pair<bool,int> hitsTarget(const Pt2i& launchSpeed, const Pt2i& targetmin, const Pt2i& targetmax)
{
    Pt2i speed = launchSpeed;
    Pt2i pos(0,0);
    int highesty = 0;
    for (;;)
    {
        pos.x += speed.x;
        pos.y += speed.y;
        if (pos.y > highesty)
            highesty = pos.y;

        if (pos.x > targetmax.x)
            break;
        if ((pos.x >= targetmin.x) && (pos.y >= targetmin.y) && (pos.y <= targetmax.y))
        {
            return { true, highesty };
        }
        if (pos.y < targetmin.y)
            break;

        if (speed.x != 0)
            speed.x -= (speed.x / abs(speed.x));
        speed.y--;
    }

    return { false, highesty };
}

pair<Pt2i, Pt2i> parseInput(const string& input)
{
    Pt2i targetmin, targetmax;
    istringstream is(input);
    is >> "target area: x=" >> targetmin.x >> ".." >> targetmax.x >> ", y=" >> targetmin.y >> ".." >> targetmax.y;
    return {targetmin, targetmax};
}

pair<bool, int> hitsTarget(Pt2i speed, const string& input)
{
    auto [targetmin,targetmax] = parseInput(input);
    return hitsTarget(speed, targetmin, targetmax);
}


int day17(const string& input)
{
    auto [targetmin, targetmax] = parseInput(input);

    int minY = targetmin.y;
    int numYVals = targetmax.x * 5 - minY;
    vector<int16_t> testYVals(numYVals);
    iota(begin(testYVals), end(testYVals), int16_t(minY));

    int highesty = transform_reduce(execution::par_unseq,
        begin(testYVals), end(testYVals), 
        0, [](int acc, int val) { return max(acc, val); },
        [&](int16_t y) -> int
        {
            int highesty = 0;
            for (int x = 1; x < targetmax.x; ++x)
            {
                auto [hit, height] = hitsTarget({ x,y }, targetmin, targetmax);
                if (hit && height > highesty)
                {
                    highesty = height;
                }
            }
            return highesty;
        });
    
    return highesty;
}

int day17_2(const string& input)
{
    auto [targetmin, targetmax] = parseInput(input);

    int minY = targetmin.y * 2;
    int numYVals = targetmax.x * 6 + 20 - minY;
    vector<int16_t> testYVals(numYVals);
    iota(begin(testYVals), end(testYVals), int16_t(minY));

    int numHits = transform_reduce(execution::par_unseq,
        begin(testYVals), end(testYVals),
        0, plus<int>(),
        [&](int16_t y) -> int
        {
            int numHits = 0;
            for (int x = 1; x <= targetmax.x; ++x)
            {
                auto [hit, height] = hitsTarget({ x,y }, targetmin, targetmax);
                if (hit)
                    ++numHits;
            }
            return numHits;
        });
    return numHits;
}


void run_day17()
{
    string sample = "target area: x=20..30, y=-10..-5";

    test(true, hitsTarget({ 7,2 }, sample).first);
    test(true, hitsTarget({ 6,3 }, sample).first);
    test(true, hitsTarget({ 9,0 }, sample).first);
    test(false, hitsTarget({ 17,-4 }, sample).first);
    test(true, hitsTarget({ 6,9 }, sample).first);
    test(45, hitsTarget({ 6,9 }, sample).second);
    test(45, day17(sample));
    gogogo(day17(LOADSTR(17)), 2775);

    test(112, day17_2(sample));
    gogogo(day17_2(LOADSTR(17)), 1566);
}