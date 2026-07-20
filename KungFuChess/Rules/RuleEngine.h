#pragma once
#include "../Model/Board.h"
#include "MoveValidation.h"
#include "../Model/Position.h"
#include <vector>

class RuleEngine {
public:
    static MoveValidation validateMove(const Board& board, const Position& src, const Position& dst);
    static std::vector<Position> legalDestinations(const Board& board, const Position& src);
};
