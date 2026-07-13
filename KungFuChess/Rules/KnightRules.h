#pragma once
#include "PieceRules.h"

// Knight: L-shaped jumps, ignoring blockers.
class KnightRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
