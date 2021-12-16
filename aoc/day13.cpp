#include "pch.h"
#include "harness.h"
#include "pt2.h"

using Pt = Pt2i16;
using Fold = pair<char, int16_t>;

pair<vector<Pt2i16>, vector<Fold>> loadOrigami(const stringlist& input)
{
    vector<Pt> points;
    points.reserve(input.size());
    vector<Fold> folds;
    for (auto& line : input)
    {
        if (line.empty())
            continue;

        istringstream is(line);
        if (line[0] == 'f')
        {
            Fold f;
            is >> "fold along " >> f.first >> "=" >> f.second;
            folds.push_back(f);
        }
        else
        {
            Pt p;
            is >> p.x >> "," >> p.y;
            points.push_back(p);
        }
    }

    return {points, folds};
}

int day13(const stringlist& input, int maxFolds=1)
{
    auto [points, folds] = loadOrigami(input);

    maxFolds = min(maxFolds, int(folds.size()));
    for (int i=0; i<maxFolds; ++i)
    {
        Pt::el_type* pval = &points.front().x;
        if (folds[i].first == 'y')
            ++pval;

        auto fold = folds[i].second;
        for ( ; pval < &points.back().y + 2; pval += 2)
        {
            if (*pval > fold)
                *pval = (2 * fold) - *pval;
        }
    }

    ranges::sort(points);
    auto ded = ranges::unique(points);
    return int(distance(points.begin(), ded.begin()));
}

int day13_2(const stringlist& input)
{
    auto [points, folds] = loadOrigami(input);

    for (const Fold& fold : folds)
    {
        Pt::el_type* pval = &points.front().x;
        if (fold.first == 'y')
            ++pval;

        auto foldLine = fold.second;
        for (; pval < &points.back().y + 1; pval += 2)
        {
            if (*pval > foldLine)
                *pval = (2 * foldLine) - *pval;
        }
    }

    ranges::sort(points);
    auto ded = ranges::unique(points);
    points.erase(ded.begin(), ded.end());

    int maxx = points.back().x; // points are sorted by x
    auto itMaxy = ranges::max_element(points, {}, &Pt::y);
    int maxy = itMaxy->y;

    int width=maxx+1, height=maxy+1;

    vector<char> screen(width*height, ' ');
    for (const Pt& p : points)
        screen[p.x + p.y*width] = '#';

    for (int y=0; y<height; ++y)
    {
        string_view line(screen.data() + y * width, width);
        cout << DARK_RED << line << '\n';
    }
    cout << RESET << endl;

    return 1337;
}


void run_day13()
{
    string sample =
R"(6,10
0,14
9,10
0,3
10,4
4,11
6,0
6,12
4,1
0,13
10,12
3,4
3,0
8,4
1,10
2,14
8,10
9,0

fold along y=7
fold along x=5)";

    test(17, day13(READ(sample), 1));
    gogogo(day13(LOAD(13), 1));

    test(1337, day13_2(READ(sample)));
    gogogo(day13_2(LOAD(13)));
}