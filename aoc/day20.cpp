#include "pch.h"
#include "harness.h"
#include <bitset>

using EnhanceRules = vector<int8_t>;

__forceinline int8_t charToBin(char c)
{
    return (c == '#') ? 1 : 0;
}

class InfinImg
{
    using Cell = int8_t;
    vector<Cell>    m_cells;
    size_t          m_width;
    size_t          m_height;
    Cell            m_boundaryVal = 0;
    int             m_borderRemaining;

public:
    InfinImg(span<const string> input, int initialBorder)
        : m_borderRemaining(initialBorder)
    {
        size_t inWidth = input.front().size();
        m_width = inWidth + 2 * initialBorder;
        size_t inHeight = input.size();
        m_height = input.size() + 2 * initialBorder;

        m_cells.resize(m_width * m_height, 0);

        auto itLine = begin(input);
        for(size_t y=0; y<inHeight; ++y, ++itLine)
        {
            auto itRowOut = begin(m_cells) + (initialBorder + (y+initialBorder) * m_width);
            ranges::transform(*itLine, itRowOut, charToBin);
        }
    }

    void enhance(const EnhanceRules& rules);
    size_t count() const { return accumulate(begin(m_cells), end(m_cells), 0); }

    friend ostream& operator<<(ostream& os, const InfinImg& img);
};

void InfinImg::enhance(const EnhanceRules& inRules)
{
    _ASSERT(m_borderRemaining >= 2);
    auto rules = data(inRules);

    Cell newBoundary = m_boundaryVal ? rules[511] : rules[0];
    vector<Cell> newCells(m_width * m_height, newBoundary);

    size_t border = m_borderRemaining - 2;

    int ul = -int(m_width) - 1;
    int up = -int(m_width);
    int ur = -int(m_width) + 1;
    int lf = -1;
    int rt = 1;
    int dl = int(m_width) - 1;
    int dn = int(m_width);
    int dr = int(m_width) + 1;

    for (size_t y = border; y < m_height - border; ++y)
    {
        auto itIn = data(m_cells) + (border + y * m_width);
        auto itOut = data(newCells) + (border + y * m_width);

        for (size_t x = border; x < m_width - border; ++x, ++itIn, ++itOut)
        {
            uint32_t lookup = 0;
            lookup |= uint32_t(*(itIn + ul)) << 8;
            lookup |= uint32_t(*(itIn + up)) << 7;
            lookup |= uint32_t(*(itIn + ur)) << 6;
            lookup |= uint32_t(*(itIn + lf)) << 5;
            lookup |= uint32_t(*(itIn +  0)) << 4;
            lookup |= uint32_t(*(itIn + rt)) << 3;
            lookup |= uint32_t(*(itIn + dl)) << 2;
            lookup |= uint32_t(*(itIn + dn)) << 1;
            lookup |= uint32_t(*(itIn + dr));

            *itOut = rules[lookup];
        }
    }

    m_boundaryVal = newBoundary;
    m_cells.swap(newCells);
    m_borderRemaining = int(border);
}


ostream& operator<<(ostream& os, const InfinImg& img)
{
    auto* pcurr = img.m_cells.data();
    for (size_t y = 0; y < img.m_height; ++y)
    {
        for (size_t x = 0; x < img.m_width; ++x, ++pcurr)
        {
            cout << (*pcurr ? '#' : '.');
        }
        cout << endl;
    }
    return os;
}


pair<EnhanceRules, InfinImg> parseInput(const stringlist& input, int initialBorder)
{
    EnhanceRules rules;
    rules.reserve(input.front().length());
    ranges::transform(input.front(), back_inserter(rules), charToBin);
    _ASSERT(rules.size() == 512);

    InfinImg map(span<const string>(begin(input) + 2, input.size() - 2), initialBorder);

    return { rules, map };
}

int day20(const stringlist& input, int numEnhancements = 2)
{
    auto [rules, map] = parseInput(input, 2 * (numEnhancements + 1));

    //cout << "initial: ----------------\n" << map << endl;
    for (int i = 1; i <= numEnhancements; ++i)
    {
        map.enhance(rules);
        //cout << "after " << i << ": ----------------\n" << map << endl;
    }

    return int(map.count());
}


void run_day20()
{
    string sample =
R"(..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..###..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###.######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#..#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#......#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#.....####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.......##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#

#..#.
#....
##..#
..#..
..###)";

    test(35, day20(READ(sample)));
    gogogo(day20(LOAD(20)));

    test(3351, day20(READ(sample), 50));
    gogogo(day20(LOAD(20), 50));
}