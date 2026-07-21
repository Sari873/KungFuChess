#include "KingRules.h"
#include "../GameConstants.h"

std::vector<Position> KingRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    for (int dr = Kfc::Grid::kKingNeighborMin; dr <= Kfc::Grid::kKingNeighborMax; ++dr) {
        for (int dc = Kfc::Grid::kKingNeighborMin; dc <= Kfc::Grid::kKingNeighborMax; ++dc) {
            if (dr == Kfc::Grid::kNeutral && dc == Kfc::Grid::kNeutral) continue;
            Position dst(src.getRow() + dr, src.getCol() + dc);
            if (board.inBounds(dst)) {
                res.push_back(dst);
            }
        }
    }
    return res;
}
