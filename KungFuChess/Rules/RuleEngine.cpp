#include "RuleEngine.h"
#include "BishopRules.h"
#include "KingRules.h"
#include "KnightRules.h"
#include "PawnRules.h"
#include "QueenRules.h"
#include "RookRules.h"

const PieceRules& RuleEngine::rulesFor(PieceKind kind) {
    static const RookRules rook;
    static const BishopRules bishop;
    static const QueenRules queen;
    static const KnightRules knight;
    static const KingRules king;
    static const PawnRules pawn;

    switch (kind) {
        case PieceKind::Rook:   return rook;
        case PieceKind::Bishop: return bishop;
        case PieceKind::Queen:  return queen;
        case PieceKind::Knight: return knight;
        case PieceKind::King:   return king;
        case PieceKind::Pawn:   return pawn;
    }
    return rook; // unreachable - every PieceKind is handled above
}

MoveValidation RuleEngine::validateMove(const Board& board, const Position& src, const Position& dst) {
    if (!board.inBounds(src) || !board.inBounds(dst)) {
        return MoveValidation::fail("outside_board");
    }

    if (board.isEmpty(src)) {
        return MoveValidation::fail("empty_source");
    }

    if (!board.isEmpty(dst) && board.isFriendly(src, dst)) {
        return MoveValidation::fail("friendly_destination");
    }

    const Piece* piece = board.getPieceAt(src);
    const PieceRules& rules = rulesFor(piece->getKind());
    std::vector<Position> destinations = rules.legalDestinations(*piece, board);

    for (const Position& candidate : destinations) {
        if (candidate == dst) {
            return MoveValidation::ok();
        }
    }
    return MoveValidation::fail("illegal_piece_move");
}
