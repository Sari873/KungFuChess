#include "QueenRules.h"

std::vector<Position> QueenRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();

    // Rook-style directions
    castRay(src, -1, 0, board, res);
    castRay(src, 1, 0, board, res);
    castRay(src, 0, -1, board, res);
    castRay(src, 0, 1, board, res);

    // Bishop-style directions
    castRay(src, -1, -1, board, res);
    castRay(src, -1, 1, board, res);
    castRay(src, 1, -1, board, res);
    castRay(src, 1, 1, board, res);

    return res;
}
