#include "BoardMapper.h"

bool BoardMapper::pixelsToCell(int x, int y, const Board& board, Position& outPos) {
    if (x < 0 || y < 0) return false;

    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;
    Position candidate(row, col);

    if (!board.inBounds(candidate)) return false;

    outPos = candidate;
    return true;
}
