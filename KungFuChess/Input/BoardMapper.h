#pragma once
#include "../Model/Board.h"
#include "../Model/Position.h"

class BoardMapper {
public:
    static constexpr int CELL_SIZE = 100;

    static bool pixelsToCell(int x, int y, const Board& board, Position& outPos);
};
