#include "KingRules.h"

std::vector<Position> KingRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            Position dst(src.getRow() + dr, src.getCol() + dc);
            if (board.inBounds(dst)) {
                res.push_back(dst);
            }
        }
    }
    return res;
}
