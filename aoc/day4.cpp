#include "pch.h"
#include "harness.h"

#include <ranges>

struct Board
{
    using Cell = uint8_t;
    static const int kSize = 5;
    static const Cell kDrawn = (1 << (sizeof(Cell) * 8 - 1));

    Cell cells[kSize * kSize];   // MSB is 1 if number has been drawn

    Board(stringlist::const_iterator& itLine)
    {
        auto pCell = &cells[0];
        for (int row = 0; row < kSize; ++row)
        {
            for (int col = 0; col < kSize; ++col)
            {
                *pCell = Cell(strtol(itLine->c_str() + (col*3), nullptr, 10));
                ++pCell;
            }
            ++itLine;
        }
    }

    bool onDraw(int drawn)
    {
        for (Cell& cell : cells)
        {
            if (cell == drawn)
            {
                cell |= kDrawn;

                return wasWinningCell(cell);
            }
        }
        return false;
    }

    bool wasWinningCell(const Cell& cell)
    {
        int cellIx = int(&cell - &cells[0]);
        int cellRow = cellIx % kSize;
        int cellCol = cellIx / kSize;

        // check col
        bool won = true;
        for (int row = 0; row < kSize; ++row)
        {
            if ((cells[row + (cellCol * kSize)] & kDrawn) == 0)
            {
                won = false;
                break;
            }
        }
        if (won)
            return true;

        // check row
        won = true;
        for (int col = 0; col < kSize; ++col)
        {
            if ((cells[cellRow + (col * kSize)] & kDrawn) == 0)
            {
                won = false;
                break;
            }
        }
        return won;
    }

    int getBoardScore() const
    {
        int total = 0;
        for (Cell cell : cells)
        {
            if ((cell & kDrawn) == 0)
                total += cell;
        }
        return total;
    }
};

vector<Board> loadBoards(const stringlist& input)
{
    vector<Board> boards;
    for (auto itLine = input.begin() + 2; itLine != input.end(); )
    {
        if (itLine->length() == 0)
        {
            ++itLine;
            continue;
        }

        boards.emplace_back(itLine);    // nb. increments itLine
    }
    return boards;
}

int day4(const stringlist& input)
{
    vector<int> draws;
    ranges::transform(split(input[0], ","), back_inserter(draws), [](const string& draw) { return stoi(draw); });

    auto boards = loadBoards(input);

    for (auto draw : draws)
    {
        for (Board& board : boards)
        {
            if (board.onDraw(draw))
            {
                return board.getBoardScore() * draw;
            }
        }
    }

    return -1;
}

int day4_2(const stringlist& input)
{
    vector<int> draws;
    ranges::transform(split(input[0], ","), back_inserter(draws), [](const string& draw) { return stoi(draw); });

    auto boards = loadBoards(input);

    auto itDraw = draws.begin();
    vector<vector<Board>::iterator> winningBoards;
    while (true)
    {
        for (auto itBoard = boards.begin(); itBoard != boards.end(); ++itBoard)
        {
            if (itBoard->onDraw(*itDraw))
            {
                winningBoards.push_back(itBoard);
            }
        }
        while (!winningBoards.empty() && boards.size() > 1)
        {
            boards.erase(winningBoards.back());
            winningBoards.pop_back();
        }
        if (!winningBoards.empty())
        {
            return boards.front().getBoardScore() * *itDraw;
        }
        
        ++itDraw;
    }
    return -1;
}


void run_day4()
{
    string sample =
R"(7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1

22 13 17 11  0
 8  2 23  4 24
21  9 14 16  7
 6 10  3 18  5
 1 12 20 15 19

 3 15  0  2 22
 9 18 13 17  5
19  8  7 25 23
20 11 10 24  4
14 21 16 12  6

14 21 17 24  4
10 16 15  9 19
18  8 23 26 20
22 11 13  6  5
 2  0 12  3  7)";

    test(4512, day4(READ(sample)));
    gogogo(day4(LOAD(4)));

    test(1924, day4_2(READ(sample)));
    gogogo(day4_2(LOAD(4)));
}