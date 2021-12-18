#include "pch.h"
#include "harness.h"
#include <variant>

// some trickiness to allow a recursive variant
struct SnailShell;
using SnailEl = variant<unique_ptr<SnailShell>, int64_t>;
using SnailN = pair<SnailEl, SnailEl>;
struct SnailShell
{
    SnailN n;

    SnailShell(SnailN&& _n) noexcept : n(move(_n)) { /**/ }
    ~SnailShell() = default;
    operator const SnailN&() const { return n; }
};


SnailN parseSnailNumber(const char*& pcurr);

SnailEl parseSnailElement(const char*& pcurr)
{
    if (*pcurr == '[')
        return make_unique<SnailShell>(move(parseSnailNumber(pcurr)));

    int64_t n = strtoll(pcurr, &const_cast<char*&>(pcurr), 10);
    return { n };
}

SnailN parseSnailNumber(const char*& pcurr)
{
    _ASSERT(*pcurr == '[');
    ++pcurr;

    SnailEl first = parseSnailElement(pcurr);
    _ASSERT(*pcurr == ',');
    ++pcurr;

    SnailEl second = parseSnailElement(pcurr);
    _ASSERT(*pcurr == ']');
    ++pcurr;

    return {move(first), move(second)};
}

ostream& operator<<(ostream& os, const SnailN& n)
{
    os << "(";
    if (holds_alternative<int64_t>(n.first))
        os << get<int64_t>(n.first);
    else
        os << get<unique_ptr<SnailShell>>(n.first).get()->n;
    os << ",";
    if (holds_alternative<int64_t>(n.second))
        os << get<int64_t>(n.second);
    else
        os << get<unique_ptr<SnailShell>>(n.second).get()->n;
    os << ")";
    return os;
}

int day18(const stringlist& input)
{
    for (auto& line : input)
    {
        auto pcurr = line.c_str();
        auto n = parseSnailNumber(pcurr);
        cout << "parsed " << n << " from " << line << endl;
    }

    return -1;
}

int day18_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day18()
{
    string sample =
R"([1,2]
[[1,2],3]
[9,[8,7]]
[[1,9],[8,5]]
[[[[1,2],[3,4]],[[5,6],[7,8]]],9]
[[[9,[3,8]],[[0,9],6]],[[[3,7],[4,9]],3]]
[[[[1,3],[5,3]],[[1,3],[8,7]]],[[[4,9],[6,9]],[[8,2],[7,3]]]])";

    test(-100, day18(READ(sample)));
    //gogogo(day18(LOAD(18)));

    //test(-100, day18_2(READ(sample)));
    //gogogo(day18_2(LOAD(18)));
}