#include "RookRules.h"
#include "../GameConstants.h"

std::vector<Position> RookRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    castRay(src, Kfc::Grid::kBackward, Kfc::Grid::kNeutral, board, res);
    castRay(src, Kfc::Grid::kForward, Kfc::Grid::kNeutral, board, res);
    castRay(src, Kfc::Grid::kNeutral, Kfc::Grid::kBackward, board, res);
    castRay(src, Kfc::Grid::kNeutral, Kfc::Grid::kForward, board, res);

    return res;
}
