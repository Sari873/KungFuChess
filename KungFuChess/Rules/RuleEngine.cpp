#include "RuleEngine.h"
#include "PieceRulesRegistry.h"

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

    const std::vector<Position> destinations = legalDestinations(board, src);
    for (const Position& candidate : destinations) {
        if (candidate == dst) {
            return MoveValidation::ok();
        }
    }
    return MoveValidation::fail("illegal_piece_move");
}

std::vector<Position> RuleEngine::legalDestinations(const Board& board, const Position& src) {
    if (!board.inBounds(src) || board.isEmpty(src)) {
        return {};
    }

    const Piece* piece = board.getPieceAt(src);
    const PieceRules& rules = PieceRulesRegistry::instance().get(piece->getKind());
    return rules.legalDestinations(*piece, board);
}
