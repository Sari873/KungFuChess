#pragma once
#include "../Model/Position.h"
#include "../GameConstants.h"
#include <algorithm>
#include <cmath>
#include <vector>

class MotionPath {
public:
    static constexpr long long MS_PER_CELL = Kfc::Timing::kMsPerCell;

    MotionPath(const Position& src, const Position& dst, long long startMs,
               long long msPerCell = MS_PER_CELL);

    const Position& getSrc() const { return src_; }
    const Position& getDst() const { return dst_; }

    long long getStartMs() const { return startMs_; }
    long long getDurationMs() const { return durationMs_; }
    long long getArrivalMs() const { return arrivalMs_; }

    bool hasArrived(long long nowMs) const { return nowMs >= arrivalMs_; }

    double progress(long long nowMs) const;

    const std::vector<Position>& waypoints() const { return waypoints_; }

    long long enterTime(std::size_t waypointIndex) const;

    Position cellAt(long long nowMs) const;

    bool occupies(const Position& cell, long long nowMs) const;

    int waypointIndexAtEnterTime(long long timeMs) const;

    Position previousCell(std::size_t waypointIndex) const;

    void truncateTo(const Position& newDst, long long newArrivalMs);

private:
    static std::vector<Position> buildWaypoints(const Position& src, const Position& dst);

    Position src_;
    Position dst_;
    long long startMs_;
    long long durationMs_;
    long long arrivalMs_;
    long long msPerCell_;
    std::vector<Position> waypoints_;
};
