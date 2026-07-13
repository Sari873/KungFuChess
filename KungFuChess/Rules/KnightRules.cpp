#include "KnightRules.h"

std::vector<Position> KnightRules::legalDestinations(const Piece& piece, const Board& board) const {
    static const int offsets[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        { 1, -2}, { 1, 2}, { 2, -1}, { 2, 1}
    };

    std::vector<Position> res;
    Position src = piece.getCell();

    for (const auto& off : offsets) {
        Position dst(src.getRow() + off[0], src.getCol() + off[1]);
        if (board.inBounds(dst)) {
            // Knights jump over blockers - occupancy is never checked here.
            res.push_back(dst);
        }
    }
    return res;
}
