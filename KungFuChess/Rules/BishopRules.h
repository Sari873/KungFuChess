#pragma once
#include "PieceRules.h"

// Bishop: diagonal sliding until blocked.
class BishopRules : public PieceRules {
public:
    std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const override;
};
