#include "pch.h"
#include "harness.h"

#define UPR(c)    (c & ~0x20)
#define LWR(c)    (c | 0x20)
#define ISLWR(c)  ((c & 0x20) != 0)

struct Room
{
    char spaces[2];
    char type;
};

bool canMoveInto(char amph, const Room& r)
{
    if (r.type != amph)
        return false;

    if (r.spaces[0] != '_' && UPR(r.spaces[0]) != amph)
        return false;
    if (r.spaces[1] != '_' && UPR(r.spaces[1]) != amph)
        return false;

    return true;
}

int64_t moveInto(char amph, Room& r)
{
    if (r.spaces[1] == '_')
    {
        r.spaces[1] = LWR(amph);
        return 2;
    }

    if (r.spaces[0] == '_')
    {
        r.spaces[0] = LWR(amph);
        return 1;
    }

    _ASSERT(false);
    return INT_MAX;
}

struct Hall
{
    char spaces[12];
    Room rooms[4];

    Hall(const stringlist& input)
    {
        ranges::fill(spaces, '_');
        spaces[11] = 0;
        for (int i=0; i<4; ++i)
        {
            auto& r = rooms[i];
            r.type = char('A' + i);
            r.spaces[0] = input[2][3 + 2 * i];
            r.spaces[1] = input[3][3 + 2 * i];

            if (r.spaces[1] == r.type)
            {
                r.spaces[1] = LWR(r.type);
                if (r.spaces[0] == r.type)
                    r.spaces[0] = LWR(r.type);
            }
        }
    }
};


struct Place
{
    uint8_t inroom : 1; // if false, it's in the hall
    uint8_t room : 2;
    uint8_t space : 4;
};

struct Open
{
    char  amph;
    Place from;
    Place to;
};

struct OpenSet
{
    enum { Max = 48 };
    Open    buf[Max];
    int8_t  used;

    OpenSet() : used(0)
    {
        memset(buf, 0, sizeof(buf));
    }

    void pushFromHall(char amph, uint8_t fromSpace, uint8_t toRoom, uint8_t toSpace)
    {
        _ASSERT(used < Max);
        buf[used].amph = amph;

        buf[used].from.inroom = 0;
        buf[used].from.room = 0;
        buf[used].from.space = fromSpace;

        buf[used].to.inroom = 1;
        buf[used].to.room = toRoom;
        buf[used].to.space = toSpace;

        ++used;
    }

    void pushFromRoom(char amph, uint8_t fromRoom, uint8_t fromSpace, uint8_t toHallSpace)
    {
        _ASSERT(used < Max);
        buf[used].amph = amph;

        buf[used].from.inroom = 1;
        buf[used].from.room = fromRoom;
        buf[used].from.space = fromSpace;

        buf[used].to.inroom = 0;
        buf[used].to.room = 0;
        buf[used].to.space = toHallSpace;

        ++used;
    }

    Open pop()
    {
        _ASSERT(used > 0);
        return buf[used--];
    }

    bool empty() const  { return used == 0; }

    const Open& back() const
    {
        _ASSERT(used > 0);
        return buf[used];
    }
};

struct State
{
    int64_t score;
    Hall h;
    OpenSet open;

    void pushValidMovesFromHall(uint8_t hallSpace)
    {
        char amph = h.spaces[hallSpace];

        // we can move into any open room
    }

    State(int64_t score, const Hall& hall) : score(score), h(hall)
    {
        for (char* c = h.spaces; *c; ++c)
        {
            if (*c != '_')
                open.pushHall(uint8_t(c - h.spaces), *c);
        }
        for (uint8_t i=0; i<4; ++i)
        {
            const Room& r = h.rooms[i];
            if (r.spaces[0] != '_' && !ISLWR(r.spaces[0]))
            {
                open.pushRoom(i, 0, r.spaces[0]);
                continue;
            }
            if (r.spaces[1] != '_' && !ISLWR(r.spaces[1]))
            {
                open.pushRoom(i, 1, r.spaces[1]);
                continue;
            }
        }
    }

    bool isComplete() const
    {
        for (auto& r : h.rooms)
        {
            if (!ISLWR(r.spaces[0]))
                return false;
        }
        return true;
    }
};

const int64_t MoveCost[4] = { 1, 10, 100, 1000 };

ostream& operator<<(ostream& os, const Hall& h)
{
    os << h.spaces << "\n";
    os << "  ";
    for (const Room& r : h.rooms)
        os << r.spaces[0] << ' ';
    os << "\n  ";
    for (const Room& r : h.rooms)
        os << r.spaces[1] << ' ';
    os << endl;
    return os;
}

int64_t day23(const stringlist& input)
{
    Hall h(input);
    cout << h;

    vector<State> best;
    int64_t bestScore = numeric_limits<int64_t>::max();
    vector<State> states;
    states.emplace_back(0, h);
    while (states.empty())
    {
        auto& state = states.back();

        if (state.isComplete())
        {
            if (state.score < bestScore)
            {
                best = states;
                bestScore = state.score;
                states.pop_back();
                continue;
            }
        }
        if (state.open.empty())
        {
            states.pop_back();
            continue;
        }

        Open move = state.open.pop();

    }

    return bestScore;
}

int day23_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day23()
{
    string sample =
R"(#############
#...........#
###B#C#B#D###
  #A#D#C#A#
  #########)";

    test(-100, day23(READ(sample)));
    //gogogo(day23(LOAD(23)));

    //test(-100, day23_2(READ(sample)));
    //gogogo(day23_2(LOAD(23)));
}