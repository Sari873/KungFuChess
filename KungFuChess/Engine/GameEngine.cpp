#include "GameEngine.h"
#include "../GameConstants.h"
#include "../Rules/RuleEngine.h"
#include "../Rules/PromotionRules.h"
#include "../Model/Piece.h"

GameEngine::GameEngine() {}

GameEngine::GameEngine(GameState state) : state_(std::move(state)) {}

long long GameEngine::getClockMs() const { return clockMs_; }

bool GameEngine::isGameOver() const { return state_.isGameOver(); }

std::optional<PieceColor> GameEngine::winner() const { return state_.winner(); }

bool GameEngine::hasActiveMotions() const { return arbiter_.hasActiveMotions(); }

bool GameEngine::hasActiveJumps() const { return arbiter_.hasActiveJumps(); }

const Board& GameEngine::getBoard() const { return state_.getBoard(); }

bool GameEngine::isMoving(const Position& cell) const {
    return arbiter_.isMoving(state_.getBoard(), cell);
}

bool GameEngine::isJumping(const Position& cell) const {
    return arbiter_.isJumping(state_.getBoard(), cell);
}

std::vector<MotionSnapshot> GameEngine::activeMotions() const {
    return arbiter_.snapshots();
}

std::vector<JumpSnapshot> GameEngine::activeJumps() const {
    return arbiter_.jumpSnapshots();
}

MoveResult GameEngine::requestMove(const Position& src, const Position& dst) {
    if (state_.isGameOver()) {
        return MoveResult::rejected("game_over");
    }

    Board& board = state_.getBoard();

    if (arbiter_.isMoving(board, src)) {
        return MoveResult::rejected("motion_in_progress");
    }
    if (arbiter_.isJumping(board, src)) {
        return MoveResult::rejected("jump_in_progress");
    }

    MoveValidation validation = RuleEngine::validateMove(board, src, dst);
    if (!validation.is_valid) {
        return MoveResult::rejected(validation.reason);
    }

    if (!arbiter_.startMotion(board, src, dst, clockMs_)) {
        return MoveResult::rejected("cannot_start_motion");
    }
    return MoveResult::accepted();
}

MoveResult GameEngine::requestJump(const Position& cell) {
    if (state_.isGameOver()) {
        return MoveResult::rejected("game_over");
    }

    Board& board = state_.getBoard();
    const Piece* piece = board.getPieceAt(cell);
    if (piece == nullptr) {
        return MoveResult::rejected("empty_source");
    }
    if (piece->getState() == PieceState::Captured) {
        return MoveResult::rejected("captured");
    }
    if (arbiter_.isPieceMoving(piece->getId()) || piece->getState() == PieceState::Moving) {
        return MoveResult::rejected("motion_in_progress");
    }
    if (arbiter_.isPieceJumping(piece->getId()) || piece->getState() == PieceState::Jumping) {
        return MoveResult::rejected("jump_in_progress");
    }

    if (!arbiter_.startJump(board, cell, clockMs_)) {
        return MoveResult::rejected("cannot_start_jump");
    }
    return MoveResult::accepted();
}

std::vector<Position> GameEngine::legalDestinations(const Position& src) const {
    return RuleEngine::legalDestinations(state_.getBoard(), src);
}

void GameEngine::advanceTime(long long deltaMs) {
    if (deltaMs <= Kfc::Timing::kNonPositiveDeltaMs) return;

    clockMs_ += deltaMs;

    Board& board = state_.getBoard();
    ArrivalReport report = arbiter_.advanceTo(clockMs_, board);

    for (const CaptureEvent& capture : report.captures) {
        if (capture.capturedKind == PieceKind::King) {
            const PieceColor winner = (capture.capturedColor == PieceColor::White)
                ? PieceColor::Black
                : PieceColor::White;
            state_.endGame(winner);
        }
    }

    for (const ArrivalEvent& arrival : report.arrivals) {
        Piece* piece = board.getPieceAt(arrival.cell);
        if (piece == nullptr || piece->getId() != arrival.pieceId) continue;

        if (PromotionRules::shouldPromote(*piece, arrival.cell, board)) {
            piece->setKind(PromotionRules::promotedKind());
        }
    }
}
