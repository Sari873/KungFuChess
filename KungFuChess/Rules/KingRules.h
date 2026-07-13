#pragma once
#include "PieceRules.h"

// King: one square in any direction.
class KingRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
