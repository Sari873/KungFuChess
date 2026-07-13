#include "BishopRules.h"

std::vector<Position> BishopRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    castRay(src, -1, -1, board, res);
    castRay(src, -1, 1, board, res);
    castRay(src, 1, -1, board, res);
    castRay(src, 1, 1, board, res);

    return res;
}
