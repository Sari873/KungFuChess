#include "PieceRules.h"

void PieceRules::castRay(const Position& src, int dRow, int dCol, const Board& board, std::vector<Position>& out) {
    Position cur(src.getRow() + dRow, src.getCol() + dCol);

    while (board.inBounds(cur)) {
        out.push_back(cur);
        if (!board.isEmpty(cur)) break; // blocker included, but stop here
        cur = Position(cur.getRow() + dRow, cur.getCol() + dCol);
    }
}
