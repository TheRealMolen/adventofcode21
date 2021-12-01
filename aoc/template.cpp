#include "pch.h"
#include "harness.h"


int day7(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}

int day7_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day7()
{
    string sample =
        R"()";

    test(-100, day7(READ(sample)));
    //gogogo(day7(LOAD(7)));

    //test(-100, day7_2(READ(sample)));
    //gogogo(day7_2(LOAD(7)));
}