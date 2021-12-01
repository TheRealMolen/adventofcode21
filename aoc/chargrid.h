#pragma once

#include <algorithm>
#include <ostream>
#include <string>
#include <ranges>
#include <vector>

using namespace std;

using CharGrid = vector<string>;


ostream& operator<<(ostream& os, const CharGrid& grid)
{
    for (const auto& row : grid)
        os << row << '\n';
    return os;
}


inline string get_column(size_t ix, const CharGrid& grid)
{
    string column(grid.size(), '.');
    ranges::transform(grid, column.begin(), [ix](const string& row) { return row[ix]; });
    return column;
}

inline CharGrid rotate(const CharGrid& grid, uint32_t turns_anticlockwise)
{
    CharGrid rotated;

    switch (turns_anticlockwise & 0x03)
    {
    case 0:
        copy(grid.begin(), grid.end(), back_inserter(rotated));
        break;

    case 1:
        for (size_t col = grid.front().size() - 1; col < grid.front().size(); --col)
            rotated.emplace_back(get_column(col, grid));
        break;

    case 2:
        for (auto itrow = grid.rbegin(); itrow != grid.rend(); ++itrow)
            rotated.emplace_back(itrow->rbegin(), itrow->rend());
        break;

    case 3:
        for (size_t col = 0; col < grid.front().size(); ++col)
        {
            string c = get_column(col, grid);
            rotated.emplace_back(c.rbegin(), c.rend());
        }
        break;
    }

    return rotated;
}

inline void vflip(CharGrid& grid)
{
    ranges::reverse(grid);
}

inline void hflip(CharGrid& grid)
{
    for (string& row : grid)
        ranges::reverse(row);
}


