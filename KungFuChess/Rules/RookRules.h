#pragma once
#include "PieceRules.h"

class RookRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
