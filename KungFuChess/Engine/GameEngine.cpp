#include "GameEngine.h"
#include "../Rules/RuleEngine.h"
#include "../Model/Piece.h"

GameEngine::GameEngine() {}

GameEngine::GameEngine(GameState state) : state_(std::move(state)) {}

long long GameEngine::getClockMs() const { return clockMs_; }

bool GameEngine::isGameOver() const { return state_.isGameOver(); }

const Board& GameEngine::getBoard() const { return state_.getBoard(); }

bool GameEngine::isMoving(const Position& cell) const {
    return arbiter_.isMoving(state_.getBoard(), cell);
}

MoveResult GameEngine::requestMove(const Position& src, const Position& dst) {
    // Application-level guard first. RuleEngine never learns that game_over
    // exists.
    if (state_.isGameOver()) {
        return MoveResult::rejected("game_over");
    }

    Board& board = state_.getBoard();

    // A piece already travelling cannot be commanded again.
    if (arbiter_.isMoving(board, src)) {
        return MoveResult::rejected("motion_in_progress");
    }

    // Chess legality is delegated. RuleEngine is read-only over the board.
    MoveValidation validation = RuleEngine::validateMove(board, src, dst);
    if (!validation.is_valid) {
        return MoveResult::rejected(validation.reason);
    }

    // Valid: hand it to the arbiter as a motion. The board does not change yet.
    arbiter_.startMotion(board, src, dst, clockMs_);
    return MoveResult::accepted();
}

void GameEngine::advanceTime(long long deltaMs) {
    if (deltaMs <= 0) return;

    clockMs_ += deltaMs;

    std::vector<CaptureEvent> captures = arbiter_.advanceTo(clockMs_, state_.getBoard());

    // The arbiter reports captures; deciding what a captured king MEANS is an
    // application-level concern, so it is resolved here and not in the arbiter.
    for (const CaptureEvent& capture : captures) {
        if (capture.capturedKind == PieceKind::King) {
            state_.setGameOver(true);
        }
    }
}
