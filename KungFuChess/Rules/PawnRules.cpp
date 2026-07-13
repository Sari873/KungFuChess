#include "PawnRules.h"

std::vector<Position> PawnRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();
    int dir = (piece.getColor() == PieceColor::White) ? -1 : 1;

    // Forward move: only onto an empty cell, never a capture.
    Position forward(src.getRow() + dir, src.getCol());
    if (board.inBounds(forward) && board.isEmpty(forward)) {
        res.push_back(forward);
    }

    // Diagonal capture: only onto a cell occupied by an enemy piece.
    for (int dc : {-1, 1}) {
        Position diag(src.getRow() + dir, src.getCol() + dc);
        if (!board.inBounds(diag)) continue;

        const Piece* target = board.getPieceAt(diag);
        if (target != nullptr && target->getColor() != piece.getColor()) {
            res.push_back(diag);
        }
    }

    return res;
}
