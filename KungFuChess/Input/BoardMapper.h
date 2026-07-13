#pragma once
#include "Board.h"
#include "Position.h"

class BoardMapper {
public:
    static constexpr int CELL_SIZE = 100;

    // Converts pixel coordinates to a board cell. Returns false when the
    // pixel coordinates fall outside the board; outPos is left unspecified
    // in that case.
    static bool pixelsToCell(int x, int y, const Board& board, Position& outPos);
};
