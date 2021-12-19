#include "pch.h"
#include "harness.h"

string reparen(const string& in)
{
    string n = in;
    replace(begin(n), end(n), '[', '(');
    replace(begin(n), end(n), ']', ')');
    return n;
}
void reparen(char* s)
{
    for (char* c = s; *c; ++c)
    {
        if (*c == '[')
            *c = '(';
        else if (*c == ']')
            *c = ')';
    }
}
void deparen(char* s)
{
    for (char* c = s; *c; ++c)
    {
        if (*c == '(')
            *c = '[';
        else if (*c == ')')
            *c = ']';
    }
}

__forceinline bool isRegular(char c)
{
    return c >= '0';
}
__forceinline bool isOversize(char c)
{
    return c > '9';
}

char* findRegular(char* c)
{
    for (; *c; ++c)
    {
        if (isRegular(*c))
            return c;
    }
    return nullptr;
}

char* findOversize(char* c)
{
    for (; *c; ++c)
    {
        if (isOversize(*c))
            return c;
    }
    return nullptr;
}

bool tryExplode(string& n)
{
    int nesting = 0;
    char* buf = n.data();
    char* prevDigit = nullptr;
    for (char* it = buf; *it; ++it)
    {
        if (*it == '(')
            ++nesting;
        else if (*it == ')')
            --nesting;
        else if (*it >= '0')
        {
            if (nesting > 4 && (*(it+1) == ',') && isRegular(*(it+2)))
            {
                auto startOfPair = it;
                auto first = it;
                auto second = it + 2;
                auto endOfPair = it + 3;
                if (prevDigit)
                {
                    // add first digit to prev digit
                    *prevDigit += (*first - '0');
                }
                auto nextDigit = findRegular(endOfPair);
                if (nextDigit)
                {
                    *nextDigit += (*second - '0');
                }

                // replace the whole pair with "0"
                *(startOfPair-1) = '0';
                n.erase(distance(buf, first), (endOfPair+1) - first);
                return true;
            }
            else
            {
                prevDigit = it;
            }
        }
    }
    return false;
}

bool trySplit(string& n)
{
    char* it = findOversize(n.data());
    if (!it)
        return false;

    int val = *it - '0';
    int left = val / 2;
    int right = val - left;
    char p[] = { char(left + '0'), ',', char(right + '0'), ')', 0 };

    *it = '(';
    n.insert(distance(n.data(), it+1), p);
    return true;
}

string testExplode(const string& in)
{
    string s = reparen(in);
    tryExplode(s);
    deparen(s.data());
    return s;
}

string snailAdd(const string& l, const string& r)
{
    string res("[");
    res.reserve(4000);
    res.append(l);
    res.append(",");
    res.append(r);
    res.append("]");
    reparen(res.data());

    for (;;)
    {
        if (tryExplode(res))
            continue;
        if (trySplit(res))
            continue;
        break;
    }

    deparen(res.data());
    return res;
}

string addLines(const stringlist& input)
{
    string acc;
    acc.reserve(4000);
    acc = input.front();
    for (auto it = begin(input) + 1; it != end(input); ++it)
    {
        acc = snailAdd(acc, *it);
    }
    return acc;
}

int64_t magnitude(const char*& pcurr)
{
    if (*pcurr != '[')
    {
        int64_t val = *pcurr - '0';
        ++pcurr;
        return val;
    }

    ++pcurr;

    int64_t first = magnitude(pcurr);
    _ASSERT(*pcurr == ',');
    ++pcurr;

    int64_t second = magnitude(pcurr);
    _ASSERT(*pcurr == ']');
    ++pcurr;

    return first * 3 + second * 2;
}

int64_t magnitude(const string& s)
{
    auto pcurr = s.c_str();
    return magnitude(pcurr);
}

int64_t day18(const stringlist& input)
{
    string final = addLines(input);
    return magnitude(final);
}

int64_t day18_2(const stringlist& input)
{
    int64_t biggest = -1;
    for (auto first = begin(input); first != end(input); ++first)
    {
        for (auto second = begin(input); second != end(input); ++second)
        {
            if (first == second)
                continue;

            string added = snailAdd(*first, *second);
            auto mag = magnitude(added);
            if (mag > biggest)
                biggest = mag;
        }
    }
 
    return biggest;
}


void run_day18()
{
    string sample1 =
R"([1,1]
[2,2]
[3,3]
[4,4]
[5,5]
[6,6])";
    string sample2 =
R"([[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]
[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]
[[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]
[[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]
[7,[5,[[3,8],[1,4]]]]
[[2,[2,2]],[8,[8,1]]]
[2,9]
[1,[[[9,3],9],[[9,0],[0,7]]]]
[[[5,[7,4]],7],1]
[[[[4,2],2],6],[8,7]])";
    string sample3 =
R"([[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
[[[5,[2,8]],4],[5,[[9,9],0]]]
[6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
[[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
[[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
[[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
[[[[5,4],[7,7]],8],[[8,3],8]]
[[9,3],[[9,9],[6,[4,9]]]]
[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
[[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]])";

    test<string>("[[[[0,9],2],3],4]", testExplode("[[[[[9,8],1],2],3],4]"));
    test<string>("[7,[6,[5,[7,0]]]]", testExplode("[7,[6,[5,[4,[3,2]]]]]"));
    test<string>("[[6,[5,[7,0]]],3]", testExplode("[[6,[5,[4,[3,2]]]],1]"));
    test<string>("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]", testExplode("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"));
    test<string>("[[3,[2,[8,0]]],[9,[5,[7,0]]]]", testExplode("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"));
    test<string>("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]", snailAdd("[[[[4,3],4],4],[7,[[8,4],9]]]", "[1,1]"));
    test<string>("[[[[5,0],[7,4]],[5,5]],[6,6]]", addLines(READ(sample1)));
    test<string>("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]", addLines(READ(sample2)));
    test<int64_t>(29, magnitude("[9,1]"));
    test<int64_t>(21, magnitude("[1,9]"));
    test<int64_t>(143, magnitude("[[1,2],[[3,4],5]]"));
    test<int64_t>(3488, magnitude("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]"));
    test<int64_t>(4140, day18(READ(sample3)));
    gogogo(day18(LOAD(18)));

    test<int64_t>(3993, day18_2(READ(sample3)));
    gogogo(day18_2(LOAD(18)));
}
