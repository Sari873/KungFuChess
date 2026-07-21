#pragma once

#include "DisplayTypes.h"
#include <optional>
#include <vector>

struct BoardRenderHints {
    std::optional<CellCoord> selected;
    std::vector<CellCoord> legalMoves;
    long long clockMs = 0;
    std::vector<DisplayMotion> motions;
    std::vector<DisplayJump> jumps;
    bool gameOver = false;
    std::optional<DisplayColor> winner;
};
