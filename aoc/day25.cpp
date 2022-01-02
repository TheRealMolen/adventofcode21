#include "pch.h"
#include "harness.h"

struct Cucumbers
{
    int width, height;
    vector<char> cells;

    Cucumbers(const stringlist& input)
    {
        width = int(input.front().size());
        height = int(input.size());

        cells.reserve(width * height);

        auto back = back_inserter(cells);
        for (const string& line : input)
            ranges::copy(line, back);
    }

    int runToEnd();

    bool tickEast(vector<char>& next);
    bool tickSouth(vector<char>& next);
};

bool Cucumbers::tickEast(vector<char>& next)
{
    bool moved = false;

    next.clear();
    auto back = back_inserter(next);
    auto curr = begin(cells);
    for (int y = 0; y < height; ++y)
    {
        bool lastSpaceMoved = false;

        if (*curr == '.' && *(curr + (width - 1)) == '>')
        {
            *(back++) = '>';
            moved = true;
            lastSpaceMoved = true;
        }
        else if (*curr == '>' && *(curr + 1) == '.')
            *(back++) = '.';
        else
            *(back++) = *curr;
        ++curr;

        for (int x = 1; x < width - 1; ++x, ++curr)
        {
            if (*curr == '.' && *(curr - 1) == '>')
            {
                *(back++) = '>';
                moved = true;
            }
            else if (*curr == '>' && *(curr + 1) == '.')
                *(back++) = '.';
            else
                *(back++) = *curr;
        }

        if (lastSpaceMoved)
            *(back++) = '.';
        else if (*curr == '.' && *(curr - 1) == '>')
        {
            *(back++) = '>';
            moved = true;
        }
        else
            *(back++) = *curr;
        ++curr;
    }

    cells.swap(next);

    return moved;
}

bool Cucumbers::tickSouth(vector<char>& next)
{
    bool moved = false;

    int lastRowDist = (height - 1) * width;

    next.clear();
    auto curr = begin(cells);
    vector<bool> lastSpaceMoved(width, false);

    // first row
    for (int x = 0; x < width; ++x, ++curr)
    {
        if (*curr == '.' && *(curr + lastRowDist) == 'v')
        {
            next.push_back('v');
            moved = true;
            lastSpaceMoved[x] = true;
        }
        else if (*curr == 'v' && *(curr + width) == '.')
            next.push_back('.');
        else
            next.push_back(*curr);
    }

    // middle
    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 0; x < width; ++x, ++curr)
        {
            if (*curr == '.' && *(curr - width) == 'v')
            {
                next.push_back('v');
                moved = true;
            }
            else if (*curr == 'v' && *(curr + width) == '.')
                next.push_back('.');
            else
                next.push_back(*curr);
        }
    }

    // last row
    for (int x = 0; x < width; ++x, ++curr)
    {
        if (lastSpaceMoved[x])
            next.push_back('.');
        else if (*curr == '.' && *(curr - width) == 'v')
        {
            next.push_back('v');
            moved = true;
        }
        else
            next.push_back(*curr);
    }

    cells.swap(next);

    return moved;
}


int Cucumbers::runToEnd()
{
    bool moved;
    vector<char> next;
    next.reserve(cells.size());
    int numSteps = 0;
    do
    {
        moved = tickEast(next);
        moved |= tickSouth(next);

        ++numSteps;
    } while (moved);

    return numSteps;
}


int day25(const stringlist& input)
{
    Cucumbers cucs(input);

    int steps = cucs.runToEnd();
    return steps;
}


void run_day25()
{
    string sample =
R"(v...>>.vv>
.vv>>.vv..
>>.>v>...v
>>v>>.>.v.
v>v.vv.v..
>.>>..v...
.vv..>.>v.
v.v..>>v.v
....v..v.>)";

    test(58, day25(READ(sample)));
    gogogo(day25(LOAD(25)));

    gogogo("we can start this thing remotely!");
}