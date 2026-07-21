#include "BoardMapper.h"
#include "../GameConstants.h"

bool BoardMapper::pixelsToCell(int x, int y, int rows, int cols, CellCoord& outPos) {
    if (x < Kfc::Grid::kNeutral || y < Kfc::Grid::kNeutral) return false;

    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;
    CellCoord candidate(row, col);

    if (row < Kfc::Grid::kNeutral || row >= rows || col < Kfc::Grid::kNeutral || col >= cols) return false;

    outPos = candidate;
    return true;
}

void BoardMapper::cellToPixels(const CellCoord& cell, int& outX, int& outY) {
    outX = cell.col * CELL_SIZE;
    outY = cell.row * CELL_SIZE;
}
