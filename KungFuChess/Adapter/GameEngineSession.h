#pragma once

#include "../Engine/GameEngine.h"
#include "../Presentation/IGameSession.h"

class GameEngineSession final : public IGameSession {
public:
    explicit GameEngineSession(GameEngine& engine);

    void advanceTime(long long deltaMs) override;
    long long clockMs() const override;

    bool isGameOver() const override;
    std::optional<DisplayColor> winner() const override;

    DisplayBoard board() const override;

    bool isEmpty(const CellCoord& cell) const override;
    bool isMoving(const CellCoord& cell) const override;
    bool isJumping(const CellCoord& cell) const override;

    void requestMove(const CellCoord& src, const CellCoord& dst) override;
    void requestJump(const CellCoord& cell) override;
    std::vector<CellCoord> legalDestinations(const CellCoord& src) const override;

    std::vector<DisplayMotion> activeMotions() const override;
    std::vector<DisplayJump> activeJumps() const override;

    GameEngine& engine() { return engine_; }
    const GameEngine& engine() const { return engine_; }

private:
    GameEngine& engine_;
};
