#include "pch.h"
#include "harness.h"


int day9(const stringlist& input)
{
    int width = (int)input.front().length();
    int height = (int)input.size();

    int risk = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int n = input[y][x];
            if (y > 0 && input[y - 1][x] <= n)
                continue;
            if (y+1 < height && input[y + 1][x] <= n)
                continue;
            if (x > 0 && input[y][x - 1] <= n)
                continue;
            if (x+1 < width && input[y][x + 1] <= n)
                continue;

            risk += 1 + (n - '0');
        }
    }

    return risk;
}


class MapD9
{
    using Cell = int8_t;
    vector<Cell> m_cells;

    uint32_t m_width;
    uint32_t m_height;

public:
    MapD9(const stringlist& input)
    {
        m_width = (uint32_t)input.front().length();
        m_height = (uint32_t)input.size();

        m_cells.reserve(m_width * m_height);

        auto back = back_inserter(m_cells);
        for (const string& line : input)
            ranges::copy(line, back);
    }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    int findBasinSize(int x, int y) const;

    vector<int> getBasinSizes() const;
};

int MapD9::findBasinSize(int bx, int by) const
{
    vector<pair<int16_t,int16_t>> openCells;
    openCells.emplace_back(int16_t(bx), int16_t(by));

    int size = 0;
    vector<bool> visited(m_width * m_height, false);
    auto tryvisit = [&, this](int16_t x, int16_t y) -> void
    {
        if (x < 0 || y < 0 || uint32_t(x) >= m_width || uint32_t(y) >= m_height)
            return;

        size_t ix = x + y * m_width;
        _ASSERT(ix < m_cells.size());
        if (visited[ix])
            return;

        if (m_cells[ix] == '9')
            return;

        openCells.emplace_back(x, y);
    };

    while(!openCells.empty())
    {
        auto [x,y] = openCells.back();
        openCells.pop_back();

        if (visited[x + y * m_width])
            continue;

        ++size;
        visited[x + y*m_width] = true;

        tryvisit(x - 1, y);
        tryvisit(x + 1, y);
        tryvisit(x, y - 1);
        tryvisit(x, y + 1);
    }

    return size;
}

vector<int> MapD9::getBasinSizes() const
{
    vector<int> basinSizes;
    const Cell* curr = m_cells.data();

    for (int16_t y = 0; y < int(m_height); ++y)
    {
        for (int16_t x = 0; x < int(m_width); ++x, ++curr)
        {
            int n = *curr;
            if (y > 0 && *(curr - m_width) <= n)
                continue;
            if (y + 1 < int(m_height) && *(curr + m_width) <= n)
                continue;
            if (x > 0 && *(curr - 1) <= n)
                continue;
            if (x + 1 < int(m_width) && *(curr + 1) <= n)
                continue;

            basinSizes.push_back(findBasinSize(x, y));
        }
    }

    return basinSizes;
}


int day9_2(const stringlist& input)
{
    MapD9 map(input);
    auto basinSizes = map.getBasinSizes();
    ranges::nth_element(basinSizes, begin(basinSizes) + 2, greater{});
    auto result = accumulate(begin(basinSizes), begin(basinSizes) + 3, 1, [](int acc, int size) { return acc * size; });
    return result;
}


void run_day9()
{
    string sample =
R"(2199943210
3987894921
9856789892
8767896789
9899965678)";

    test(15, day9(READ(sample)));
    gogogo(day9(LOAD(9)));

    test(1134, day9_2(READ(sample)));
    gogogo(day9_2(LOAD(9)));
}