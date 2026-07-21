#pragma once
#include "../Model/Position.h"
#include "../GameConstants.h"

struct Jump {
    static constexpr long long DURATION_MS = Kfc::Timing::kJumpDurationMs;

    int pieceId = 0;
    Position cell;
    long long startMs = 0;

    long long landMs() const { return startMs + DURATION_MS; }

    bool isAirborneAt(long long nowMs) const {
        return nowMs >= startMs && nowMs < landMs();
    }
};

struct JumpSnapshot {
    int pieceId = 0;
    Position cell;
    long long startMs = 0;
    long long landMs = 0;

    double progressAt(long long nowMs) const {
        return Kfc::Progress::fromElapsedDuration(nowMs - startMs, landMs - startMs);
    }
};
