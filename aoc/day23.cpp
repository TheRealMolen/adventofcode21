#include "pch.h"
#include "harness.h"

#include <mutex>
#include <thread>

#define AS_AWAY(c)    (c & ~0x20)
#define AS_HOME(c)    (c | 0x20)
#define IS_HOME(c)  ((c) >= 'a' && (c) <= 'd')
#define IS_AWAY(c)  ((c) >= 'A' && (c) <= 'D')

#define ROOMIX_TO_POS(ix)   ((ix) * 2 + 2)

static constexpr unsigned HallSize = 11;

mutex g_logMutex;


template<unsigned RoomSize = 2>
struct Room
{
    enum { Size = RoomSize };

    char spaces[Size];
};

template<unsigned _RoomSize = 2>
struct Hall
{
    static const uint8_t RoomCount = 4;
    static const uint8_t RoomSize = _RoomSize;

    char spaces[HallSize + 1];
    uint8_t awayRemaining = RoomSize * RoomCount;
    Room<RoomSize> rooms[RoomCount];
    //const Hall* prev = nullptr;

    Hall(const stringlist& input)
    {
        strcpy_s(spaces, "..!.!.!.!..");
        spaces[11] = 0;
        for (int i=0; i<RoomCount; ++i)
        {
            auto& r = rooms[i];
            auto type = char('A' + i);
            for (uint32_t roomSpace = 0; roomSpace < RoomSize; ++roomSpace)
                r.spaces[roomSpace] = input[2 + roomSpace][3 + 2 * i];

            for (int roomSpace = RoomSize-1; roomSpace >= 0; --roomSpace)
            {
                if (r.spaces[roomSpace] != type)
                    break;

                
                --awayRemaining;
                r.spaces[roomSpace] = AS_HOME(r.spaces[roomSpace]);
            }
        }
    }
};

template<unsigned RoomSize>
ostream& operator<<(ostream& os, const Hall<RoomSize>& h)
{
    os << h.spaces << "\n";
    os << "  ";
    for (uint32_t i = 0; i < RoomSize; ++i)
    {
        for (const Room<RoomSize>& r : h.rooms)
            os << r.spaces[i] << ' ';
        os << "\n  ";
    }
    return os;
}

const int64_t MoveCost[4] = { 1, 10, 100, 1000 };

template<unsigned RoomSize>
void doNextMove(const Hall<RoomSize>& prev, int64_t currCost, int64_t& bestCost);

template<unsigned RoomSize>
inline void tryMoveFromHall(const Hall<RoomSize>& prev, uint8_t hallSpace, int64_t currCost, int64_t& bestCost)
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

    auto& room = prev.rooms[roomIx];
    if (room.spaces[0] != '.')
        return;
    uint8_t roomSpace = 0;
    for (uint8_t i = 1; i < RoomSize; ++i)
    {
        if (IS_AWAY(room.spaces[i]))
            return;
        if (room.spaces[i] == '.')
            roomSpace = i;
    }

    auto moveCost = MoveCost[amph - 'A'];
    auto distance = roomSpace + 1 + uint8_t(abs(roomPos - hallSpace));
    auto newCost = currCost + distance * moveCost;

    Hall h(prev);
    h.spaces[hallSpace] = '.';
    h.rooms[roomIx].spaces[roomSpace] = AS_HOME(amph);
    --h.awayRemaining;

    if (h.awayRemaining == 0)
    {
        if (newCost < bestCost)
        {
            bestCost = newCost;

            //lock_guard<mutex> cs(g_logMutex);
            //cout << "found a solution costing " << newCost << ":\n" << h << endl;
        }
    }
    else
    {
        doNextMove(h, newCost, bestCost);
    }
}

template<unsigned RoomSize>
inline void moveFromRoomToHall(const Hall<RoomSize>& prev, uint8_t roomIx, uint8_t roomSpace, uint8_t hallSpace, int64_t currCost, int64_t& bestCost)
{
    char amph = prev.rooms[roomIx].spaces[roomSpace];

    _ASSERT(IS_AWAY(amph));
    auto moveCost = MoveCost[amph - 'A'];
    auto roomPos = ROOMIX_TO_POS(roomIx);
    auto distance = roomSpace + 1 + uint8_t(abs(roomPos - hallSpace));
    auto newCost = currCost + distance * moveCost;

    Hall h(prev);
    h.rooms[roomIx].spaces[roomSpace] = '.';
    h.spaces[hallSpace] = amph;

    doNextMove(h, newCost, bestCost);
}

template<unsigned RoomSize>
inline void tryMoveFromRoom(const Hall<RoomSize>& prev, uint8_t roomIx, int64_t currCost, int64_t& bestCost)
{
    auto& room = prev.rooms[roomIx];
    uint8_t roomSpace = RoomSize;
    for (uint8_t i=0; i<RoomSize; ++i)
    {
        char amph = room.spaces[i];
        if (IS_AWAY(amph))
        {
            roomSpace = i;
            break;
        }

        if (IS_HOME(amph))
            return;

        _ASSERT(amph == '.');
    }
    if (roomSpace == RoomSize)
        return;

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

template<unsigned RoomSize>
void doNextMove(const Hall<RoomSize>& prev, int64_t currCost, int64_t& bestCost)
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


template<unsigned RoomSize>
void doFirstMove(int move, const Hall<RoomSize>& prev, int64_t* bestCost)
{
    uint8_t roomIx = uint8_t(move / 7);
    switch(move % 7)
    {
    case 0: moveFromRoomToHall(prev, roomIx, 0, 0, 0, *bestCost); break;
    case 1: moveFromRoomToHall(prev, roomIx, 0, 1, 0, *bestCost); break;
    case 2: moveFromRoomToHall(prev, roomIx, 0, 3, 0, *bestCost); break;
    case 3: moveFromRoomToHall(prev, roomIx, 0, 5, 0, *bestCost); break;
    case 4: moveFromRoomToHall(prev, roomIx, 0, 7, 0, *bestCost); break;
    case 5: moveFromRoomToHall(prev, roomIx, 0, 9, 0, *bestCost); break;
    case 6: moveFromRoomToHall(prev, roomIx, 0, 10, 0, *bestCost); break;
    }
}


template<unsigned RoomSize>
int64_t findBestSolution(const Hall<RoomSize>& initial)
{
    static const int NumInitialMoves = 4 * 7;
    vector<int64_t> bestCosts(NumInitialMoves, INT64_MAX);
    vector<thread> threads;
    for (int i=0; i<NumInitialMoves; ++i)
        threads.push_back(thread(doFirstMove<RoomSize>, i, initial, &bestCosts[i]));

    for (auto& thr : threads)
        thr.join();

    int64_t bestCost = ranges::min(bestCosts);
    return bestCost;
}


int64_t day23(const stringlist& input)
{
    Hall<2> h(input);
    int64_t bestCost = INT64_MAX;
    bestCost = findBestSolution(h);

    return bestCost;
}

int64_t day23_2(stringlist input)
{
    input.insert(begin(input) + 3, "  #D#B#A#C#");
    input.insert(begin(input) + 3, "  #D#C#B#A#");

    Hall<4> h(input);
    int64_t bestCost = INT64_MAX;
    bestCost = findBestSolution(h);

    return bestCost;
}


void run_day23()
{
    string sample =
R"(#############
#...........#
###B#C#B#D###
  #A#D#C#A#
  #########)";

    nestD(12521, day23(READ(sample)));
    nonono(day23(LOAD(23)), 15472ll);

    nest(44169, day23_2(READ(sample)));
    nonono(day23_2(LOAD(23)));
}