#include "pch.h"
#include "harness.h"

#define AS_AWAY(c)    (c & ~0x20)
#define AS_HOME(c)    (c | 0x20)
#define IS_HOME(c)  ((c) >= 'a' && (c) <= 'd')
#define IS_AWAY(c)  ((c) >= 'A' && (c) <= 'D')

#define ROOMIX_TO_POS(ix)   ((ix) * 2 + 2)

static constexpr unsigned HallSize = 11;


struct Room
{
    char spaces[2];
    //char type;
    //uint8_t pos;
};

struct Hall
{
    char spaces[HallSize + 1];
    uint8_t awayRemaining = 8;
    Room rooms[4];
    const Hall* prev = nullptr;

    Hall(const stringlist& input)
    {
        strcpy_s(spaces, "..!.!.!.!..");
        spaces[11] = 0;
        for (int i=0; i<4; ++i)
        {
            auto& r = rooms[i];
            auto type = char('A' + i);
           // r.type = char('A' + i);
           // r.pos = 2 + i*2;
            r.spaces[0] = input[2][3 + 2 * i];
            r.spaces[1] = input[3][3 + 2 * i];

            if (r.spaces[1] == type)
            {
                --awayRemaining;
                r.spaces[1] = AS_HOME(r.spaces[1]);
                if (r.spaces[0] == type)
                {
                    --awayRemaining;
                    r.spaces[0] = AS_HOME(r.spaces[0]);
                }
            }
        }
    }

    Hall(const Hall& prev, bool link)
    {
        _ASSERT(link);
        (void)link;

        memcpy(this, &prev, sizeof(Hall));
        this->prev = &prev;
    }
};

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

const int64_t MoveCost[4] = { 1, 10, 100, 1000 };

void doNextMove(const Hall& prev, int64_t currCost, int64_t& bestCost);

inline void tryMoveFromHall(const Hall& prev, uint8_t hallSpace, int64_t currCost, int64_t& bestCost)
{
    char amph = prev.spaces[hallSpace];
    _ASSERT(amph != '!');
    if (amph == '.')
        return;

    _ASSERT(IS_AWAY(amph));
    auto roomIx = uint8_t(amph - 'A');
    auto roomPos = ROOMIX_TO_POS(roomIx);

    int d = (roomPos > hallSpace) ? 1 : -1;
    for (auto s = hallSpace + d; s != roomPos; s += d)
    {
        if (IS_AWAY(prev.spaces[s]))
            return;
    }

    const Room& room = prev.rooms[roomIx];
    if (room.spaces[0] != '.')
        return;
    if (IS_AWAY(room.spaces[1]))
        return;

    uint8_t roomSpace = (room.spaces[1] == '.') ? 1 : 0;

    auto moveCost = MoveCost[amph - 'A'];
    auto distance = roomSpace + 1 + uint8_t(abs(roomPos - hallSpace));
    auto newCost = currCost + distance * moveCost;

    Hall h(prev, true);
    h.spaces[hallSpace] = '.';
    h.rooms[roomIx].spaces[roomSpace] = AS_HOME(amph);
    --h.awayRemaining;

    if (h.awayRemaining == 0)
    {
        if (newCost < bestCost)
        {
            bestCost = newCost;
            //cout << "found a solution costing " << newCost << ":\n" << h << endl;
        }
    }
    else
    {
        doNextMove(h, newCost, bestCost);
    }
}

inline void moveFromRoomToHall(const Hall& prev, uint8_t roomIx, uint8_t roomSpace, uint8_t hallSpace, int64_t currCost, int64_t& bestCost)
{
    char amph = prev.rooms[roomIx].spaces[roomSpace];

    _ASSERT(amph - 'A' >= 0 && amph - 'A' < 4);
    auto moveCost = MoveCost[amph - 'A'];
    auto roomPos = ROOMIX_TO_POS(roomIx);
    auto distance = roomSpace + 1 + uint8_t(abs(roomPos - hallSpace));
    auto newCost = currCost + distance * moveCost;

    Hall h(prev, true);
    h.rooms[roomIx].spaces[roomSpace] = '.';
    h.spaces[hallSpace] = amph;

    doNextMove(h, newCost, bestCost);
}

inline void tryMoveFromRoom(const Hall& prev, uint8_t roomIx, int64_t currCost, int64_t& bestCost)
{
    const Room& room = prev.rooms[roomIx];
    if (IS_HOME(room.spaces[0]))
        return;

    uint8_t roomSpace = 0;
    if (IS_AWAY(room.spaces[0]))
    {
        roomSpace = 0;
    }
    else
    {
        _ASSERT(room.spaces[0] == '.');
        if (IS_HOME(room.spaces[1]) || (room.spaces[1] == '.'))
            return;
        roomSpace = 1;
    }

    // try hallway to the left
    uint8_t roomPos = ROOMIX_TO_POS(roomIx);
    for (int hallSpace = roomPos - 1; hallSpace >= 0; --hallSpace)
    {
        if (prev.spaces[hallSpace] == '!')
            continue;
        if (prev.spaces[hallSpace] != '.')
            break;

        moveFromRoomToHall(prev, roomIx, roomSpace, uint8_t(hallSpace), currCost, bestCost);
    }

    // try hallway to the right
    for (int hallSpace = roomPos + 1; hallSpace < HallSize; ++hallSpace)
    {
        if (prev.spaces[hallSpace] == '!')
            continue;
        if (prev.spaces[hallSpace] != '.')
            break;

        moveFromRoomToHall(prev, roomIx, roomSpace, uint8_t(hallSpace), currCost, bestCost);
    }
}

void doNextMove(const Hall& prev, int64_t currCost, int64_t& bestCost)
{
    tryMoveFromHall(prev, 0, currCost, bestCost);
    tryMoveFromHall(prev, 1, currCost, bestCost);
    tryMoveFromHall(prev, 3, currCost, bestCost);
    tryMoveFromRoom(prev, 0, currCost, bestCost);
    tryMoveFromHall(prev, 5, currCost, bestCost);
    tryMoveFromRoom(prev, 1, currCost, bestCost);
    tryMoveFromHall(prev, 7, currCost, bestCost);
    tryMoveFromRoom(prev, 2, currCost, bestCost);
    tryMoveFromHall(prev, 9, currCost, bestCost);
    tryMoveFromRoom(prev, 3, currCost, bestCost);
    tryMoveFromHall(prev, 10, currCost, bestCost);
}


int64_t day23(const stringlist& input)
{
    Hall h(input);
    cout << h;

    int64_t bestCost = INT64_MAX;
    doNextMove(h, 0, bestCost);

    return bestCost;
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

    test(12521, day23(READ(sample)));
    gogogo(day23(LOAD(23)));

    //test(-100, day23_2(READ(sample)));
    //gogogo(day23_2(LOAD(23)));
}