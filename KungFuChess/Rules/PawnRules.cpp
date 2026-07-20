#include "PawnRules.h"
#include "PawnGeography.h"

std::vector<Position> PawnRules::legalDestinations(const Piece& piece, const Board& board) const {
    std::vector<Position> res;
    Position src = piece.getCell();
    int dir = PawnGeography::forwardStep(piece.getColor());

    Position oneAhead(src.getRow() + dir, src.getCol());
    bool oneAheadFree = board.inBounds(oneAhead) && board.isEmpty(oneAhead);
    if (oneAheadFree) {
        res.push_back(oneAhead);

        if (src.getRow() == PawnGeography::startRow(piece.getColor(), board)) {
            Position twoAhead(src.getRow() + 2 * dir, src.getCol());
            if (board.inBounds(twoAhead) && board.isEmpty(twoAhead)) {
                res.push_back(twoAhead);
            }
        }
    }

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
