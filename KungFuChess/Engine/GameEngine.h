#pragma once
#include "../Model/GameState.h"
#include "../Model/Position.h"
#include "../Realtime/RealTimeArbiter.h"
#include "MoveResult.h"

// GameEngine is the application-service layer and the public command boundary.
// Controller and TextTestRunner talk to this, and to nothing below it.
//
// It coordinates Board, RuleEngine, and RealTimeArbiter. It owns the game clock
// and the application-level guards (game_over, motion_in_progress). It does not
// contain piece-specific movement logic, rendering code, input parsing, DSL
// parsing, or pixel mapping.
class GameEngine {
public:
    GameEngine();
    explicit GameEngine(GameState state);

    // The public command entry point. Guards -> RuleEngine -> start a motion.
    // The board is NOT mutated here: a valid move only becomes visible once
    // advanceTime() reaches its arrival.
    MoveResult requestMove(const Position& src, const Position& dst);

    // Advances simulated time. This is what makes in-flight moves land, and it
    // is the only place the board changes as a result of time passing.
    void advanceTime(long long deltaMs);

    long long getClockMs() const;
    bool isGameOver() const;

    // Read-only access for BoardPrinter and the renderer.
    const Board& getBoard() const;

    // Selection guard for Controller: a piece in flight cannot be picked up.
    bool isMoving(const Position& cell) const;

private:
    GameState state_;
    RealTimeArbiter arbiter_;
    long long clockMs_ = 0;
};
