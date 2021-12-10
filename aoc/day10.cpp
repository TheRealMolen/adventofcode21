#include "pch.h"
#include "harness.h"

#include <ranges>

int64_t getCorruptionScore(const string& line)
{
    vector<char> stack;
    stack.reserve(500);

    for (char c : line)
    {
        switch (c)
        {
        case '{': case '[': case '<': case '(':
            stack.push_back(c);
            break;

        case ')':
            if (stack.empty())
                return 0;
            if (stack.back() != '(')
                return 3;
            stack.pop_back();
            break;

        case '}':
            if (stack.empty())
                return 0;
            if (stack.back() != '{')
                return 1197;
            stack.pop_back();
            break;

        case ']':
            if (stack.empty())
                return 0;
            if (stack.back() != '[')
                return 57;
            stack.pop_back();
            break;

        case '>':
            if (stack.empty())
                return 0;
            if (stack.back() != '<')
                return 25137;
            stack.pop_back();
            break;

        default:
            throw "nope";
        }
    }

    return 0;
}


int64_t day10(const stringlist& input)
{
    int64_t corruption = 0;
    for (auto& line : input)
        corruption += getCorruptionScore(line);

    return corruption;
}

int64_t autoComplete(const string& line)
{
    vector<char> stack;
    stack.reserve(500);
    for (char c : line)
    {
        switch (c)
        {
        case '{': case '[': case '<': case '(':
            stack.push_back(c);
            break;

        case ')':
            if (stack.empty() || stack.back() != '(')
                return 0;
            stack.pop_back();
            break;

        case '}':
            if (stack.empty() || stack.back() != '{')
                return 0;
            stack.pop_back();
            break;

        case ']':
            if (stack.empty() || stack.back() != '[')
                return 0;
            stack.pop_back();
            break;

        case '>':
            if (stack.empty() || stack.back() != '<')
                return 0;
            stack.pop_back();
            break;

        default:
            throw "nope";
        }
    }

    int64_t completionScore = 0;
    for (char c : stack | views::reverse)
    {
        completionScore *= 5;

        switch (c)
        {
        case '(':
            completionScore += 1;
            break;
        case '[':
            completionScore += 2;
            break;
        case '{':
            completionScore += 3;
            break;
        case '<':
            completionScore += 4;
            break;
        }
    }
    return completionScore;
}


int64_t day10_2(const stringlist& input)
{
    vector<int64_t> scores;
    for (auto& line : input)
    {
        int64_t score = autoComplete(line);
        if (score > 0)
            scores.push_back(score);
    }

    size_t midpointIx = scores.size() / 2;
    auto midpointIt = begin(scores) + midpointIx;
    ranges::nth_element(scores, midpointIt);

    return *midpointIt;
}


void run_day10()
{
    string sample =
R"([({(<(())[]>[[{[]{<()<>>
[(()[<>])]({[<{<<[]>>(
{([(<{}[<>[]}>{[]{[(<()>
(((({<>}<{<{<>}{[]{[]{}
[[<[([]))<([[{}[[()]]]
[{[{({}]{}}([{[{{{}}([]
{<[[]]>}<{[{[{[]{()[[[]
[<(<(<(<{}))><([]([]()
<{([([[(<>()){}]>(<<{{
<{([{{}}[<[[[<>{}]]]>[]])";

    test<int64_t>(26397, day10(READ(sample)));
    gogogo(day10(LOAD(10)));

    test(288957, autoComplete("[({(<(())[]>[[{[]{<()<>>"));
    test(5566, autoComplete("[(()[<>])]({[<{<<[]>>("));
    test(288957, day10_2(READ(sample)));
    gogogo(day10_2(LOAD(10)));
}