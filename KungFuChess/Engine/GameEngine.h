#pragma once
#include "../Model/GameState.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include "../RealTime/RealTimeArbiter.h"
#include <optional>
#include <string>
#include <vector>

struct MoveResult {
    bool is_accepted;
    std::string reason;

    static MoveResult accepted() { return MoveResult{ true, "ok" }; }
    static MoveResult rejected(const std::string& reason) { return MoveResult{ false, reason }; }
};

class GameEngine {
public:
    GameEngine();
    explicit GameEngine(GameState state);

    MoveResult requestMove(const Position& src, const Position& dst);
    MoveResult requestJump(const Position& cell);

    std::vector<Position> legalDestinations(const Position& src) const;

    void advanceTime(long long deltaMs);

    long long getClockMs() const;
    bool isGameOver() const;
    std::optional<PieceColor> winner() const;
    bool hasActiveMotions() const;
    bool hasActiveJumps() const;

    const Board& getBoard() const;

    bool isMoving(const Position& cell) const;
    bool isJumping(const Position& cell) const;

    std::vector<MotionSnapshot> activeMotions() const;
    std::vector<JumpSnapshot> activeJumps() const;

private:
    GameState state_;
    RealTimeArbiter arbiter_;
    long long clockMs_ = 0;
};
