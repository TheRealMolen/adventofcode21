#include "pch.h"
#include "harness.h"

class Chitons
{
    using Cell = uint8_t;
    using Index = uint32_t;
    vector<Cell> m_cells;

    Index m_width;
    Index m_height;

public:
    Chitons(const stringlist& input)
    {
        m_width = (Index)input.front().length();
        m_height = (Index)input.size();
        _ASSERT(uint32_t(m_width) * m_height < numeric_limits<Index>::max());

        m_cells.reserve(m_width * m_height);

        auto back = back_inserter(m_cells);
        for (const string& line : input)
            ranges::transform(line, back, [](char c) { return Cell(c - '0'); });
    }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    int solve() const;
    void embiggen(uint32_t numRepeats);

    friend ostream& operator<<(ostream& os, const Chitons& map);
};

ostream& operator<<(ostream& os, const Chitons& map)
{
    const Chitons::Cell* pCurr = map.m_cells.data();
    for (size_t y = 0; y < map.m_height; ++y)
    {
        for (size_t x = 0; x < map.m_width; ++x, ++pCurr)
        {
            os << char(*pCurr + '0');
        }
        os << "\n";
    }

    return os;
}

int Chitons::solve() const
{
    using Risk = uint16_t;

    vector<Risk> minRisk(m_cells.size(), numeric_limits<Risk>::max());
    vector<Index> previousIx(m_cells.size(), 0);

    auto compareTotalRisk = [&](Index lhs, Index rhs) -> bool
    {
        return minRisk[lhs] > minRisk[rhs];
    };
    priority_queue<Index, vector<Index>, decltype(compareTotalRisk)> openIndices(compareTotalRisk);
    vector<uint8_t> isCellOpen(m_width * m_height, 0);

    auto tryVisitNeighbour = [&, this](Index currIx, int dx, int dy)
    {
        int y = currIx / m_width;
        int x = currIx % m_width;

        x += dx;
        y += dy;

        if ((x < 0) || (x >= int(m_width)) || (y < 0) || (y >= int(m_height)))
            return;
        
        Index newIx = Index(x + y * m_width);
        auto localRisk = m_cells[newIx];
        _ASSERT(minRisk[currIx] + localRisk < numeric_limits<Risk>::max());
        Risk newRisk = minRisk[currIx] + localRisk;
        if (newRisk < minRisk[newIx])
        {
            previousIx[newIx] = currIx;
            minRisk[newIx] = newRisk;

            if (!isCellOpen[newIx])
            {
                openIndices.push(newIx);
                isCellOpen[newIx] = true;
            }
        }
    };

    auto dumpPath = [&, this](Index targetIx)
    {
        if (m_width > 150)
            return;

        vector<char> buf(m_width * m_height, '.');
        for (Index ix = targetIx; ix != 0; ix = previousIx[ix])
        {
            buf[ix] = '@';
        };
        buf[0] = 'S';

        for (size_t y = 0; y < m_height; ++y)
        {
            cout << string_view(buf.data() + y * m_width, m_width) << "\n";
        }
        cout << endl;
    };

    // start at top-left (index 0)
    openIndices.push(0);
    isCellOpen[0] = true;
    minRisk[0] = 0;

    const Index targetIx = Index((m_width * m_height) - 1);

    while (!openIndices.empty())
    {
        // get the most interesting index out of the open list
        Index curr = openIndices.top();
        if (curr == targetIx)
        {
            //dumpPath(targetIx);
            return minRisk[targetIx];
        }

        openIndices.pop();
        isCellOpen[curr] = false;

        tryVisitNeighbour(curr,  1, 0);
        tryVisitNeighbour(curr, -1, 0);
        tryVisitNeighbour(curr, 0,  1);
        tryVisitNeighbour(curr, 0, -1);
    }

    return -1;
}


void Chitons::embiggen(uint32_t numRepeats)
{
    auto newWidth = Index(m_width * numRepeats);
    auto newHeight = Index(m_height * numRepeats);
    vector<Cell> newCells;
    newCells.reserve(newWidth * newHeight);
    auto back = back_inserter(newCells);
    for (Index yRepeat = 0; yRepeat < numRepeats; ++yRepeat)
    {
        for (Index y = 0; y < m_height; ++y)
        {
            for (Index xRepeat = 0; xRepeat < numRepeats; ++xRepeat)
            {
                Cell inc = (xRepeat + yRepeat) % 9;
                Cell* pOldRow = m_cells.data() + (y * m_width);
                transform(pOldRow, pOldRow + m_width, back, [&](Cell c) -> Cell
                    {
                        c += inc;
                        if (c > 9)
                            c = Cell(c - 9);
                        return c;
                    });
            }            
        }
    }

    m_width = newWidth;
    m_height = newHeight;
    m_cells.swap(newCells);
}


int day15(const stringlist& input)
{
    Chitons map(input);
    return map.solve();
}

int day15_2(const stringlist& input)
{
    Chitons map(input);
    map.embiggen(5);
    return map.solve();
}


void run_day15()
{
    string sample =
R"(1163751742
1381373672
2136511328
3694931569
7463417111
1319128137
1359912421
3125421639
1293138521
2311944581)";

    test(40, day15(READ(sample)));
    gogogo(day15(LOAD(15)));

    test(315, day15_2(READ(sample)));
    nononoD(day15_2(LOAD(15)));
}