#pragma once
#include "PieceRules.h"

// Simplified pawn: forward move (must be empty), diagonal capture (must be
// an enemy piece). No double-step, no en passant, no promotion.
//
// Orientation assumption (not stated explicitly in the design guide): White
// advances toward row 0, Black advances toward increasing row numbers -
// i.e. row 0 is White's back rank side of the printed board. If your
// board text has White on the bottom instead, flip the two directions
// below.
class PawnRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
