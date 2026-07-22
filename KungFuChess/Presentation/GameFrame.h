#pragma once

#include "BoardRenderHints.h"
#include "DisplayTypes.h"
#include "IGameSession.h"
#include <optional>
#include <vector>

struct GameFrame {
    DisplayBoard board;
    long long clockMs = 0;
    std::vector<DisplayMotion> motions;
    std::vector<DisplayJump> jumps;
    bool gameOver = false;
    std::optional<DisplayColor> winner;
};

inline GameFrame captureGameFrame(const IGameSession& session) {
    GameFrame frame;
    frame.board = session.board();
    frame.clockMs = session.clockMs();
    frame.motions = session.activeMotions();
    frame.jumps = session.activeJumps();
    frame.gameOver = session.isGameOver();
    frame.winner = session.winner();
    return frame;
}


inline BoardRenderHints toRenderHints(const GameFrame& frame,
                                      std::optional<CellCoord> selected = std::nullopt,
                                      std::vector<CellCoord> legalMoves = {}) {
    BoardRenderHints hints;
    hints.clockMs = frame.clockMs;
    hints.motions = frame.motions;
    hints.jumps = frame.jumps;
    hints.gameOver = frame.gameOver;
    hints.winner = frame.winner;
    hints.selected = std::move(selected);
    hints.legalMoves = std::move(legalMoves);
    return hints;
}
