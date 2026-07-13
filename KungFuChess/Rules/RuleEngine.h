#pragma once
#include "Board.h"
#include "MoveValidation.h"
#include "PieceRules.h"
#include "Position.h"

// RuleEngine answers one question: given a source and destination cell, is
// this move legal right now? It is read-only with respect to Board - it
// never mutates it, starts motions, or knows about game-over. Piece-specific
// movement geometry is fully delegated to the matching PieceRules subclass.
class RuleEngine {
public:
    static MoveValidation validateMove(const Board& board, const Position& src, const Position& dst);

private:
    // Returns the stateless rule-object strategy for a given piece kind.
    static const PieceRules& rulesFor(PieceKind kind);
};
