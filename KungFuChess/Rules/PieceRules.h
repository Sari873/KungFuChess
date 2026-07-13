#pragma once
#include "Board.h"
#include "Piece.h"
#include "Position.h"
#include <vector>

// PieceRules is the Strategy interface for movement geometry. Each piece
// type gets its own concrete subclass (RookRules, BishopRules, ...).
// Implementations are stateless: they do not store selected pieces, active
// motions, elapsed time, or game-over state. They only calculate legal
// destinations from the given board and piece, and never capture, remove,
// move, or mutate anything.
class PieceRules {
public:
    virtual ~PieceRules() = default;

    // Returns every cell this piece could geometrically reach on this board,
    // respecting blocking pieces where relevant. Enemy-occupied destinations
    // may be included; ownership (friendly/enemy) is NOT filtered here -
    // that is RuleEngine's responsibility.
    virtual std::vector<Position> legalDestinations(const Piece& piece, const Board& board) const = 0;

protected:
    // Shared helper for sliding pieces (rook/bishop/queen): casts a ray from
    // 'src' in direction (dRow, dCol), stopping at the board edge or right
    // after the first occupied cell (inclusive - sliding pieces do not pass
    // through blockers, but the blocker square itself is a candidate).
    static void castRay(const Position& src, int dRow, int dCol, const Board& board, std::vector<Position>& out);
};
