#pragma once
#include "../GameConstants.h"
#include "../Presentation/DisplayTypes.h"

class BoardMapper {
public:
    static constexpr int CELL_SIZE = Kfc::Ui::kCellSizePx;

    static bool pixelsToCell(int x, int y, int rows, int cols, CellCoord& outPos);
    static void cellToPixels(const CellCoord& cell, int& outX, int& outY);
};
