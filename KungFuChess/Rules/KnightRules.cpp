#include "KnightRules.h"
#include "../GameConstants.h"

std::vector<Position> KnightRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    for (const auto& off : Kfc::Knight::kOffsets) {
        Position dst(src.getRow() + off[0], src.getCol() + off[1]);
        if (board.inBounds(dst)) {
            res.push_back(dst);
        }
    }
    return res;
}
