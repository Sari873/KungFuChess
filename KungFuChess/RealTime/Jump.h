#pragma once
#include "../Model/Position.h"

struct Jump {
    static constexpr long long DURATION_MS = 1000;

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
        const long long duration = landMs - startMs;
        if (duration <= 0) {
            return 1.0;
        }
        const double p = static_cast<double>(nowMs - startMs) / static_cast<double>(duration);
        if (p < 0.0) return 0.0;
        if (p > 1.0) return 1.0;
        return p;
    }
};
