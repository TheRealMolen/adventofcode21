#include "pch.h"
#include "harness.h"

#undef small

struct Pt3i
{
    int x, y, z;

    auto operator<=>(const Pt3i&) const = default;

    Pt3i& operator+=(int t)
    {
        x += t;
        y += t;
        z += t;
        return *this;
    }
};

Pt3i minByEl(const Pt3i& a, const Pt3i& b)
{
    return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) };
}
Pt3i maxByEl(const Pt3i& a, const Pt3i& b)
{
    return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) };
}


struct Cube
{
    Pt3i lo, hi;

    auto operator<=>(const Cube&) const = default;

    Cube() = default;
    Cube(const Pt3i & l, const Pt3i & h) : lo(l), hi(h) { /**/ }
    Cube(int l, int h) : lo{l,l,l}, hi{h,h,h} { /**/ }

    bool isDistinct(const Cube& o) const
    {
        if ((lo.x >= o.hi.x) || (hi.x <= o.lo.x) ||
            (lo.y >= o.hi.y) || (hi.y <= o.lo.y) ||
            (lo.z >= o.hi.z) || (hi.z <= o.lo.z))
        {
            return true;
        }

        return false;
    }

    Cube intersectWith(const Cube& o) const
    {
        _ASSERT(!isDistinct(o));
        return { maxByEl(lo, o.lo), minByEl(hi, o.hi) };
    }

    int64_t volume() const
    {
        int64_t dx = hi.x - lo.x;
        int64_t dy = hi.y - lo.y;
        int64_t dz = hi.z - lo.z;

        return (dx * dy * dz);
    }
};

struct EngineOp
{
    Cube c;
    bool on;

    auto operator<=>(const EngineOp&) const = default;
};

struct Engine
{
    enum Constants {
        Min = -50,
        Max = 50,
        Size = (Max - Min) + 1,
    };

    using Cell = int8_t;
    using Cells = array<Cell, Size * Size * Size>;

    Cells cells;

    Engine()
    {
        memset(data(cells), 0, sizeof cells);
    }

    void switchCells(bool on, Pt3i lo, Pt3i hi);

    void diff(const Engine& o) const;

    int count() const { return accumulate(begin(cells), end(cells), 0); }
};


void Engine::switchCells(bool on, Pt3i lo, Pt3i hi)
{
    if (hi.x < Min || hi.y < Min || hi.z < Min || lo.x > Max+1 || lo.y > Max+1 || lo.z > Max+1)
        return;

    hi.x = min<int>(Max+1, hi.x);
    hi.y = min<int>(Max+1, hi.y);
    hi.z = min<int>(Max+1, hi.z);
    lo.x = max<int>(Min, lo.x);
    lo.y = max<int>(Min, lo.y);
    lo.z = max<int>(Min, lo.z);

    auto buf = data(cells);
    for (int z = lo.z - Min; z < hi.z - Min; ++z)
    {
        for (int y = lo.y - Min; y < hi.y - Min; ++y)
        {
            auto row = &buf[(y*Size) + (z*Size*Size)];
            fill(row + lo.x - Min, row + hi.x - Min, on);
        }
    }
}

void Engine::diff(const Engine& otherEngine) const
{
    const Cell* us = data(cells);
    const Cell* them = data(otherEngine.cells);
    for (int z=Min; z<=Max; ++z)
    {
        for (int y=Min; y<=Max; ++y)
        {
            for (int x=Min; x<=Max; ++x, ++us, ++them)
            {
                if (*us != *them)
                    cout << "  diff @ " << x << ", " << y << ", " << z << " -- old was " << (*them ? "ON" : "off") << "\n";
            }
        }
    }
}

EngineOp parseLine(const string& line)
{
    EngineOp op;
    istringstream is(line);
    op.on = true;

    if (line[1] == 'f')
    {
        op.on = false;
        is >> "off ";
    }
    else
    {
        is >> "on ";
    }

    auto& c = op.c;
    is >> "x=" >> c.lo.x >> ".." >> c.hi.x >> ",y=" >> c.lo.y >> ".." >> c.hi.y >> ",z=" >> c.lo.z >> ".." >> c.hi.z;

    c.hi += 1;

    return op;
}


int day22(const stringlist& input)
{
    auto engine = make_unique<Engine>();

    for (auto& line : input)
    {
        auto op = parseLine(line);
        engine->switchCells(op.on, op.c.lo, op.c.hi); // NB. modifies lo & hi
    }

    return engine->count();
}



void addRemainderOfIsxnWith(const Cube& toBeChopped, const Cube& toChopWith, vector<Cube>& cubes)
{
    Cube isxn = toBeChopped.intersectWith(toChopWith);
    Cube remain{ toBeChopped };

    // split low side of x
    if (isxn.lo.x > remain.lo.x)
    {
        cubes.emplace_back(remain.lo, Pt3i{ isxn.lo.x, remain.hi.y, remain.hi.z });
        remain.lo.x = isxn.lo.x;
    }
    // split high side of x
    if (isxn.hi.x < remain.hi.x)
    {
        cubes.emplace_back(Pt3i{ isxn.hi.x, remain.lo.y, remain.lo.z }, remain.hi);
        remain.hi.x = isxn.hi.x;
    }

    // split low side of y
    if (isxn.lo.y > remain.lo.y)
    {
        cubes.emplace_back(remain.lo, Pt3i{ remain.hi.x, isxn.lo.y, remain.hi.z });
        remain.lo.y = isxn.lo.y;
    }
    // split high side of y
    if (isxn.hi.y < remain.hi.y)
    {
        cubes.emplace_back(Pt3i{ remain.lo.x, isxn.hi.y, remain.lo.z }, remain.hi);
        remain.hi.y = isxn.hi.y;
    }

    // split low side of z
    if (isxn.lo.z > remain.lo.z)
    {
        cubes.emplace_back(remain.lo, Pt3i{ remain.hi.x, remain.hi.y, isxn.lo.z });
        remain.lo.z = isxn.lo.z;
    }
    // split high side of z
    if (isxn.hi.z < remain.hi.z)
    {
        cubes.emplace_back(Pt3i{ remain.lo.x, remain.lo.y, isxn.hi.z }, remain.hi);
        remain.hi.z = isxn.hi.z;
    }

    _ASSERT(remain == isxn);
}


static const Cube InnerCube{{-50,-50,-50},{51,51,51}};

template<bool ClipToInner = true>
int64_t day22_2(const stringlist& input)
{
    vector<Cube> regions;
    regions.reserve(input.size());

    vector<Cube> toAdd;
    for (const string& line : input)
    {
        auto op = parseLine(line);

        if constexpr (ClipToInner)
        {
            if (!op.c.isDistinct(InnerCube))
                op.c = op.c.intersectWith(InnerCube);
            else
                continue;
        }

        if (op.on)
        {
            toAdd.push_back(op.c);

            while (!toAdd.empty())
            {
                Cube c = toAdd.back();
                toAdd.pop_back();

                bool decomposed = false;
                for (Cube& o : regions)
                {
                    if (!c.isDistinct(o))
                    {
                        addRemainderOfIsxnWith(c, o, toAdd);
                        decomposed = true;
                        break;
                    }
                }

                if (!decomposed)
                    regions.push_back(c);
            }
        }
        else
        {
            bool decomposed;
            do
            {
                decomposed = false;
                for (auto itC = begin(regions); itC != end(regions); )
                {
                    if (!op.c.isDistinct(*itC))
                    {
                        Cube c = *itC;
                        itC = regions.erase(itC);
                        addRemainderOfIsxnWith(c, op.c, toAdd);
                        decomposed = true;
                        break;
                    }
                    else
                        ++itC;
                }
            } while (decomposed);

            ranges::copy(toAdd, back_inserter(regions));
            toAdd.clear();
        }
    }


    //auto oldEngine = make_unique<Engine>();
    //for (auto& line : input)
    //{
    //    auto op = parseLine(line);
    //    oldEngine->switchCells(op.on, op.c.lo, op.c.hi); // NB. modifies lo & hi
    //}
    //auto newEngine = make_unique<Engine>();
    //for (const Cube& c : regions)
    //    newEngine->switchCells(true, c.lo, c.hi);

    //newEngine->diff(*oldEngine);


    //cout << "------- remaining regions -------\n";
    //ranges::sort(regions);
    //for (const Cube& c : regions)
    //    cout << "  x=" << c.lo.x << ".." << c.hi.x << "\ty=" << c.lo.y << ".." << c.hi.y << "\tz=" << c.lo.z << ".." << c.hi.z << "\n";
    //cout << endl;

    int64_t total = 0;
    for (const Cube& c : regions)
        total += c.volume();

    return total;
}

int checkDust(const Cube& big, const Cube& small)
{
    vector<Cube> dust;
    addRemainderOfIsxnWith(big, small, dust);

    int64_t dustVol = accumulate(begin(dust), end(dust), 0ll, [](int64_t acc, const Cube& c) { return acc + c.volume(); });
    _ASSERT(dustVol + small.volume() == big.volume());

    return int(dust.size());
}


void run_day22()
{
    string sample =
R"(on x=10..12,y=10..12,z=10..12
on x=11..13,y=11..13,z=11..13
off x=9..11,y=9..11,z=9..11
on x=10..10,y=10..10,z=10..10)";

    string sample2 =
R"(on x=-20..26,y=-36..17,z=-47..7
on x=-20..33,y=-21..23,z=-26..28
on x=-22..28,y=-29..23,z=-38..16
on x=-46..7,y=-6..46,z=-50..-1
on x=-49..1,y=-3..46,z=-24..28
on x=2..47,y=-22..22,z=-23..27
on x=-27..23,y=-28..26,z=-21..29
on x=-39..5,y=-6..47,z=-3..44
on x=-30..21,y=-8..43,z=-13..34
on x=-22..26,y=-27..20,z=-29..19
off x=-48..-32,y=26..41,z=-47..-37
on x=-12..35,y=6..50,z=-50..-2
off x=-48..-32,y=-32..-16,z=-15..-5
on x=-18..26,y=-33..15,z=-7..46
off x=-40..-22,y=-38..-28,z=23..41
on x=-16..35,y=-41..10,z=-47..6
off x=-32..-23,y=11..30,z=-14..3
on x=-49..-5,y=-3..45,z=-29..18
off x=18..30,y=-20..-8,z=-3..13
on x=-41..9,y=-7..43,z=-33..15
on x=-54112..-39298,y=-85059..-49293,z=-27449..7877
on x=967..23432,y=45373..81175,z=27513..53682)";

    string sample3 =
R"(on x=-5..47,y=-31..22,z=-19..33
on x=-44..5,y=-27..21,z=-14..35
on x=-49..-1,y=-11..42,z=-10..38
on x=-20..34,y=-40..6,z=-44..1
off x=26..39,y=40..50,z=-2..11
on x=-41..5,y=-41..6,z=-36..8
off x=-43..-33,y=-45..-28,z=7..25
on x=-33..15,y=-32..19,z=-34..11
off x=35..47,y=-46..-34,z=-11..5
on x=-14..36,y=-6..44,z=-16..29
on x=-57795..-6158,y=29564..72030,z=20435..90618
on x=36731..105352,y=-21140..28532,z=16094..90401
on x=30999..107136,y=-53464..15513,z=8553..71215
on x=13528..83982,y=-99403..-27377,z=-24141..23996
on x=-72682..-12347,y=18159..111354,z=7391..80950
on x=-1060..80757,y=-65301..-20884,z=-103788..-16709
on x=-83015..-9461,y=-72160..-8347,z=-81239..-26856
on x=-52752..22273,y=-49450..9096,z=54442..119054
on x=-29982..40483,y=-108474..-28371,z=-24328..38471
on x=-4958..62750,y=40422..118853,z=-7672..65583
on x=55694..108686,y=-43367..46958,z=-26781..48729
on x=-98497..-18186,y=-63569..3412,z=1232..88485
on x=-726..56291,y=-62629..13224,z=18033..85226
on x=-110886..-34664,y=-81338..-8658,z=8914..63723
on x=-55829..24974,y=-16897..54165,z=-121762..-28058
on x=-65152..-11147,y=22489..91432,z=-58782..1780
on x=-120100..-32970,y=-46592..27473,z=-11695..61039
on x=-18631..37533,y=-124565..-50804,z=-35667..28308
on x=-57817..18248,y=49321..117703,z=5745..55881
on x=14781..98692,y=-1341..70827,z=15753..70151
on x=-34419..55919,y=-19626..40991,z=39015..114138
on x=-60785..11593,y=-56135..2999,z=-95368..-26915
on x=-32178..58085,y=17647..101866,z=-91405..-8878
on x=-53655..12091,y=50097..105568,z=-75335..-4862
on x=-111166..-40997,y=-71714..2688,z=5609..50954
on x=-16602..70118,y=-98693..-44401,z=5197..76897
on x=16383..101554,y=4615..83635,z=-44907..18747
off x=-95822..-15171,y=-19987..48940,z=10804..104439
on x=-89813..-14614,y=16069..88491,z=-3297..45228
on x=41075..99376,y=-20427..49978,z=-52012..13762
on x=-21330..50085,y=-17944..62733,z=-112280..-30197
on x=-16478..35915,y=36008..118594,z=-7885..47086
off x=-98156..-27851,y=-49952..43171,z=-99005..-8456
off x=2032..69770,y=-71013..4824,z=7471..94418
on x=43670..120875,y=-42068..12382,z=-24787..38892
off x=37514..111226,y=-45862..25743,z=-16714..54663
off x=25699..97951,y=-30668..59918,z=-15349..69697
off x=-44271..17935,y=-9516..60759,z=49131..112598
on x=-61695..-5813,y=40978..94975,z=8655..80240
off x=-101086..-9439,y=-7088..67543,z=33935..83858
off x=18020..114017,y=-48931..32606,z=21474..89843
off x=-77139..10506,y=-89994..-18797,z=-80..59318
off x=8476..79288,y=-75520..11602,z=-96624..-24783
on x=-47488..-1262,y=24338..100707,z=16292..72967
off x=-84341..13987,y=2429..92914,z=-90671..-1318
off x=-37810..49457,y=-71013..-7894,z=-105357..-13188
off x=-27365..46395,y=31009..98017,z=15428..76570
off x=-70369..-16548,y=22648..78696,z=-1892..86821
on x=-53470..21291,y=-120233..-33476,z=-44150..38147
off x=-93533..-4276,y=-16170..68771,z=-104985..-24507)";

    test(39, day22(READ(sample)));
    test(590784, day22(READ(sample2)));
    test(474140, day22(READ(sample3)));
    gogogo(day22(LOAD(22)));

    test(0, checkDust({ {-10},{10} }, { {-10},{10} }));
    test(3, checkDust({ {-10},{10} }, { {0},{10} }));
    test(6, checkDust({ {-10},{10} }, { {-1},{1} }));

    test(39ll, day22_2(READ(sample)));
    test(590784, day22_2(READ(sample2)));
    //cout.imbue(locale(""));
    test(2758514936282235ll, day22_2<false>(READ(sample3)));
    gogogo(day22_2<false>(LOAD(22)));
}