#pragma once
#include "PieceRules.h"

// Queen: rook movement plus bishop movement.
class QueenRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
