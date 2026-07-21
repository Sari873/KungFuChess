#pragma once

#include "DisplayTypes.h"
#include <optional>
#include <vector>

class IGameSession {
public:
    virtual ~IGameSession() = default;

    virtual void advanceTime(long long deltaMs) = 0;
    virtual long long clockMs() const = 0;

    virtual bool isGameOver() const = 0;
    virtual std::optional<DisplayColor> winner() const = 0;

    virtual DisplayBoard board() const = 0;

    virtual bool isEmpty(const CellCoord& cell) const = 0;
    virtual bool isMoving(const CellCoord& cell) const = 0;
    virtual bool isJumping(const CellCoord& cell) const = 0;

    virtual void requestMove(const CellCoord& src, const CellCoord& dst) = 0;
    virtual void requestJump(const CellCoord& cell) = 0;
    virtual std::vector<CellCoord> legalDestinations(const CellCoord& src) const = 0;

    virtual std::vector<DisplayMotion> activeMotions() const = 0;
    virtual std::vector<DisplayJump> activeJumps() const = 0;
};
