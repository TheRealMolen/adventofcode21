#include "pch.h"
#include "harness.h"
#include "pt2.h"


struct Line
{
    using Pt = Pt2i16;

    Pt start, end;

    Line(const string& str)
    {
        istringstream is(str);
        is >> start.x >> "," >> start.y >> " -> " >> end.x >> "," >> end.y;
    }
};

using Cell = uint16_t;
struct Map
{
    int width, height;
    vector<Cell> cells;

    Map(int _width, int _height) : width(_width), height(_height), cells(width* height, 0)
    { /**/ }

    Cell* data()                { return cells.data(); }
    const Cell* data() const    { return cells.data(); }
};

ostream& operator<<(ostream& os, const Map& map)
{
    const Cell* curr = map.data();
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x, ++curr)
        {
            _ASSERT(*curr < 10);
            if (*curr > 0)
                os << *curr;
            else
                os << '.';
        }
        os << '\n';
    }
    return os;
}


vector<Line> loadLines(const stringlist& input)
{
    vector<Line> lines;
    lines.reserve(input.size());
    ranges::transform(input, back_inserter(lines), [](const string& s) -> Line { return Line(s); });
    return lines;
}

Map createMap(const vector<Line>& lines)
{
    int maxx = -1;
    int maxy = -1;
    for (const Line& line : lines)
    {
        maxx = max(maxx, max<int>(line.start.x, line.end.x));
        maxy = max(maxy, max<int>(line.start.y, line.end.y));
    }
    return Map(maxx + 1, maxy + 1);
}

int day5(const stringlist& input)
{
    auto lines = loadLines(input);
    Map map = createMap(lines);
    auto cells = map.data();

    for (const Line& line : lines)
    {
        if (line.start.x == line.end.x)     // vertical line
        {
            auto [starty, endy] = minmax(line.start.y, line.end.y);
            Cell* start = cells + (starty * map.width) + line.start.x;
            Cell* end = start + (endy - starty + 1) * map.width;;
            for (Cell* curr = start; curr != end; curr += map.width)
                (*curr)++;
        }
        else if (line.start.y == line.end.y)    // horizontal line
        {
            auto [startx, endx] = minmax(line.start.x, line.end.x);
            Cell* start = cells + (line.start.y * map.width) + startx;
            Cell* end = start + (endx - startx + 1);
            for (Cell* curr = start; curr != end; ++curr)
                (*curr)++;
        }
    }

    return (int)ranges::count_if(map.cells, [](auto cell) { return cell > 1; });
}

int day5_2(const stringlist& input)
{
    auto lines = loadLines(input);
    Map map = createMap(lines);
    auto cells = map.data();

    for (const Line& line : lines)
    {
        if (line.start.x == line.end.x)     // vertical line
        {
            auto [starty, endy] = minmax(line.start.y, line.end.y);
            Cell* start = cells + (starty * map.width) + line.start.x;
            Cell* end = start + (endy - starty + 1) * map.width;;
            for (Cell* curr = start; curr != end; curr += map.width)
                (*curr)++;
        }
        else if (line.start.y == line.end.y)    // horizontal line
        {
            auto [startx, endx] = minmax(line.start.x, line.end.x);
            Cell* start = cells + (line.start.y * map.width) + startx;
            Cell* end = start + (endx - startx + 1);
            for (Cell* curr = start; curr != end; ++curr)
                (*curr)++;
        }
        else // diagonal
        {
            const int incy = (line.start.y < line.end.y) ? 1 : -1;
            const int incx = (line.start.x < line.end.x) ? 1 : -1;
            for (int y = line.start.y, x = line.start.x; (incy > 0 && y <= line.end.y) || (incy <= 0 && y >= line.end.y); y += incy, x += incx)
            {
                cells[x + (y * map.width)]++;
            }
        }
    }

    return (int)ranges::count_if(map.cells, [](auto cell) { return cell > 1; });
}


void run_day5()
{
    string sample =
R"(0,9 -> 5,9
8,0 -> 0,8
9,4 -> 3,4
2,2 -> 2,1
7,0 -> 7,4
6,4 -> 2,0
0,9 -> 2,9
3,4 -> 1,4
0,0 -> 8,8
5,5 -> 8,2)";

    test(5, day5(READ(sample)));
    gogogo(day5(LOAD(5)), 5294);

    test(12, day5_2(READ(sample)));
    gogogo(day5_2(LOAD(5)), 21698);
}