#pragma once
#include "PieceRules.h"

// Rook: horizontal and vertical sliding until blocked.
class RookRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
