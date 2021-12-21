#include "pch.h"
#include "harness.h"


int64_t day21(const stringlist& input)
{
    static const int nPlayers = 2;
    int pos[nPlayers];
    for (int i=0; i<nPlayers; ++i)
        pos[i] = atoi(strrchr(input[i].c_str(), ' ') + 1);

    int64_t score[nPlayers] = {0, 0};

    int die = 0;
    int64_t rollCount = 0;
    auto roll = [&]()
    {
        ++die;
        if (die > 100) die = 1;
        ++rollCount;
        return (int16_t)die;
    };

    int turn = 0;
    while (score[0] < 1000 && score[1] < 1000)
    {
        int16_t move = roll() + roll() + roll();
        move %= 10;
        pos[turn] += move;
        if (pos[turn] > 10)
            pos[turn] -= 10;
        score[turn] += pos[turn];

        turn = 1-turn;
    }

    auto loser = *ranges::min_element(score);

    return loser * rollCount;
}

struct DiracPlayer
{
    int pos;
    int score;
};

struct DiracState
{
    DiracPlayer players[2];
    int64_t count = 1;

    DiracState() = default;
    DiracState(const DiracState& prev, int turn, int roll, int64_t newCount)
    {
        players[1 - turn] = prev.players[1 - turn];

        const DiracPlayer& prevPlayer = prev.players[turn];
        int newPos = prevPlayer.pos + roll;
        if (newPos > 10)
            newPos -= 10;

        DiracPlayer& player = players[turn];
        player.score = prevPlayer.score + newPos;
        player.pos = newPos;
        count = prev.count * newCount;
    }
};

pair<int64_t,int64_t> day21_2(const stringlist& input)
{
    static const int nPlayers = 2;
    vector<DiracState> states;
    auto& state = states.emplace_back();
    for (int i = 0; i < nPlayers; ++i)
    {
        state.players[i].pos = atoi(strrchr(input[i].c_str(), ' ') + 1);
        state.players[i].score = 0;
    }

    pair<int, int> rolls[7] { {3,1}, {4,3}, {5,6}, {6,7}, {7,6}, {8,3}, {9,1} };
    int64_t wins[2] = {0,0};
    int turn = 0;
    while (!states.empty())
    {
        vector<DiracState> next;
        next.reserve(states.size() * 7);
        for (const auto& prev : states)
        {
            for (const auto& roll : rolls)
            {
                DiracState n(prev, turn, roll.first, roll.second);
                if (n.players[turn].score < 21)
                    next.push_back(n);
                else
                    wins[turn] += n.count;
            }
        }
        states.swap(next);
        turn = 1 - turn;
    }

    return {wins[0], wins[1]};
}


void run_day21()
{
    string sample =
R"(Player 1 starting position: 4
Player 2 starting position: 8)";

    test(739785, day21(READ(sample)));
    gogogo(day21(LOAD(21)));

    auto d2p2results = day21_2(READ(sample));
    test(444356092776315ll, d2p2results.first);
    test(341960390180808ll, d2p2results.second);
    gogogo(day21_2(LOAD(21)).first);
}