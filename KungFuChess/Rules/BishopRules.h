#pragma once
#include "PieceRules.h"

class BishopRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
