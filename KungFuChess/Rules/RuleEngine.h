#pragma once
#include "../Model/Board.h"
#include "MoveValidation.h"
#include "PieceRules.h"
#include "../Model/Position.h"

class RuleEngine {
public:
    static MoveValidation validateMove(const Board& board, const Position& src, const Position& dst);

private:
    static const PieceRules& rulesFor(PieceKind kind);
};
