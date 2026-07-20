#pragma once
#include "PieceRules.h"

class KingRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
