#pragma once
#include "../Model/Board.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include <vector>

class PieceRules {
public:
    virtual ~PieceRules() = default;

    virtual std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const = 0;

protected:
    static void castRay(const Position& src, int dRow, int dCol, const Board& board, std::vector<Position>& out);
};
