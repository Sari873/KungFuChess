#include "BishopRules.h"
#include "../GameConstants.h"

std::vector<Position> BishopRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    castRay(src, Kfc::Grid::kBackward, Kfc::Grid::kBackward, board, res);
    castRay(src, Kfc::Grid::kBackward, Kfc::Grid::kForward, board, res);
    castRay(src, Kfc::Grid::kForward, Kfc::Grid::kBackward, board, res);
    castRay(src, Kfc::Grid::kForward, Kfc::Grid::kForward, board, res);

    return res;
}
