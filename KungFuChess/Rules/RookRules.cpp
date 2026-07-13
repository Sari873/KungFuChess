#include "RookRules.h"

std::vector<Position> RookRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    castRay(src, -1, 0, board, res);
    castRay(src, 1, 0, board, res);
    castRay(src, 0, -1, board, res);
    castRay(src, 0, 1, board, res);

    return res;
}
