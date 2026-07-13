#pragma once
#include "../Model/GameState.h"
#include "../Model/Position.h"
#include "../RealTime/RealTimeArbiter.h"

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

    void advanceTime(long long deltaMs);

    long long getClockMs() const;
    bool isGameOver() const;

    const Board& getBoard() const;

    bool isMoving(const Position& cell) const;

private:
    GameState state_;
    RealTimeArbiter arbiter_;
    long long clockMs_ = 0;
};
