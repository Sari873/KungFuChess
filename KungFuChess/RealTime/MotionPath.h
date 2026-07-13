#pragma once
#include "../Model/Position.h"
#include <algorithm>
#include <cmath>

class MotionPath {
public:
    static constexpr long long MS_PER_CELL = 1000;

    MotionPath(const Position& src, const Position& dst, long long startMs,
        long long msPerCell = MS_PER_CELL)
        : src_(src), dst_(dst), startMs_(startMs) {
        int dr = std::abs(dst.getRow() - src.getRow());
        int dc = std::abs(dst.getCol() - src.getCol());
        int cells = std::max(dr, dc);

        durationMs_ = static_cast<long long>(cells) * msPerCell;
        arrivalMs_ = startMs_ + durationMs_;
    }

    const Position& getSrc() const { return src_; }
    const Position& getDst() const { return dst_; }

    long long getStartMs() const { return startMs_; }
    long long getDurationMs() const { return durationMs_; }
    long long getArrivalMs() const { return arrivalMs_; }

    bool hasArrived(long long nowMs) const {
        return nowMs >= arrivalMs_;
    }

    double progress(long long nowMs) const {
        if (durationMs_ <= 0) return 1.0;
        double p = static_cast<double>(nowMs - startMs_) / static_cast<double>(durationMs_);
        return std::min(1.0, std::max(0.0, p));
    }

private:
    Position src_;
    Position dst_;
    long long startMs_;
    long long durationMs_;
    long long arrivalMs_;
};
