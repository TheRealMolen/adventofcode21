#include "pch.h"
#include "harness.h"

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

pair<Cube,bool> parseLine(const string& line)
{
    istringstream is(line);
    bool on = true;

    if (line[1] == 'f')
    {
        on = false;
        is >> "off ";
    }
    else
    {
        is >> "on ";
    }

    Cube c;
    is >> "x=" >> c.lo.x >> ".." >> c.hi.x >> ",y=" >> c.lo.y >> ".." >> c.hi.y >> ",z=" >> c.lo.z >> ".." >> c.hi.z;

    c.hi += 1;

    return {c, on};
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
int64_t day22(const stringlist& input)
{
    vector<Cube> regions;
    regions.reserve(input.size());

    vector<Cube> toAdd;
    for (const string& line : input)
    {
        auto [region,on] = parseLine(line);

        if constexpr (ClipToInner)
        {
            if (!region.isDistinct(InnerCube))
                region = region.intersectWith(InnerCube);
            else
                continue;
        }

        if (on)
        {
            toAdd.push_back(region);

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
            vector<vector<Cube>::iterator> toRemove;
            for (auto itC = begin(regions); itC != end(regions); ++itC)
            {
                if (!region.isDistinct(*itC))
                {
                    Cube c = *itC;
                    toRemove.push_back(itC);
                    addRemainderOfIsxnWith(c, region, toAdd);
                }
            }

            for (auto it : toRemove | views::reverse)
                regions.erase(it);

            ranges::copy(toAdd, back_inserter(regions));
            toAdd.clear();
        }
    }

    int64_t total = 0;
    for (const Cube& c : regions)
        total += c.volume();

    return total;
}


void run_day22()
{
    string sample =
R"(on x=10..12,y=10..12,z=10..12
on x=11..13,y=11..13,z=11..13
off x=9..11,y=9..11,z=9..11
on x=10..10,y=10..10,z=10..10)";

    auto sample2 = LOAD(22t2);
    auto sample3 = LOAD(22t3);

    test(39, day22(READ(sample)));
    test(590784, day22(sample2));
    test(474140, day22(sample3));
    gogogo(day22(LOAD(22)));

    test(39ll, day22<false>(READ(sample)));
    test(590784, day22<true>(sample2));
    test(2758514936282235ll, day22<false>(sample3));
    gogogo(day22<false>(LOAD(22)));
}
