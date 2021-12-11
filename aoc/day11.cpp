#include "pch.h"
#include "harness.h"


class Octopodes
{
    using Cell = int8_t;
    using Index = uint16_t;
    vector<Cell> m_cells;

    Index m_width;
    Index m_height;

public:
    Octopodes(const stringlist& input)
    {
        m_width = (Index)input.front().length();
        m_height = (Index)input.size();

        m_cells.reserve(m_width * m_height);

        auto back = back_inserter(m_cells);
        for (const string& line : input)
            ranges::copy(line, back);
    }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    int tick();

    friend ostream& operator<<(ostream&, const Octopodes&);
};

int Octopodes::tick()
{
    vector<Index> willflash;
    willflash.reserve(100);
    ranges::for_each(m_cells, [&](Cell& c)
        {
            ++c;
            if (c == '9' + 1)
                willflash.push_back(Index(&c - m_cells.data()));
        });

    vector<bool> flashed(m_cells.size(), false);
    while (!willflash.empty())
    {
        Index ixFlash = willflash.back();
        willflash.pop_back();
        if (flashed[ixFlash])
            continue;

        int fy = ixFlash / m_width;
        int fx = ixFlash - (fy * m_width);

        flashed[ixFlash] = true;

        for (int y = max(fy-1, 0); y < min(fy+2, int(m_height)); ++y)
        {
            for (int x = max(fx - 1, 0); x < min(fx+2, int(m_width)); ++x)
            {
                if (x == fx && y == fy)
                    continue;

                Index ix = Index(x + y * m_width);
                Cell newOc = ++m_cells[ix];
                if (newOc == '9' + 1 && !flashed[ix])
                    willflash.push_back(ix);
            }
        }
    }

    int numflashes = 0;
    for (Index y = 0; y < m_height; ++y)
    {
        for (Index x = 0; x < m_width; ++x)
        {
            Index ix = x + y * m_width;
            if (flashed[ix])
            {
                m_cells[ix] = '0';
                ++numflashes;
            }
        }
    }

    return numflashes;
}

ostream& operator<<(ostream& os, const Octopodes& oct)
{
    const Octopodes::Cell* curr = oct.m_cells.data();
    for (size_t y = 0; y < oct.getHeight(); ++y)
    {
        for (size_t x = 0; x < oct.getWidth(); ++x, ++curr)
        {
            if (*curr > 0)
                os << *curr;
            else
                os << '.';
        }
        os << '\n';
    }
    return os;
}


int day11(const stringlist& input, int numticks = 100)
{
    Octopodes ocs(input);

    int totalticks = 0;
    for (int tick = 0; tick < numticks; ++tick)
    {
        totalticks += ocs.tick();
    }

    return totalticks;
}

int day11_2(const stringlist& input)
{
    Octopodes ocs(input);

    const int all = ocs.getWidth() * ocs.getHeight();
    for (int tick = 1; ; ++tick)
    {
        if (ocs.tick() == all)
            return tick;
    }

    return -1;
}


void run_day11()
{
    string sample =
R"(5483143223
2745854711
5264556173
6141336146
6357385478
4167524645
2176841721
6882881134
4846848554
5283751526)";

    test(0, day11(READ(sample), 1));
    test(35, day11(READ(sample), 2));
    test(204, day11(READ(sample), 10));
    test(1656, day11(READ(sample), 100));
    gogogo(day11(LOAD(11)));

    test(195, day11_2(READ(sample)));
    gogogo(day11_2(LOAD(11)));
}