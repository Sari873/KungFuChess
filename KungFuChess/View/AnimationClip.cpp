#include "AnimationClip.h"
#include "../GameConstants.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

AnimationClip::AnimationClip(std::vector<Img> frames, double framesPerSec, bool loop)
    : frames_(std::move(frames))
    , framesPerSec_(framesPerSec > Kfc::Progress::kMin ? framesPerSec : Kfc::Animation::kDefaultFramesPerSec)
    , loop_(loop) {
}

const Img& AnimationClip::frameAt(long long elapsedMs) const {
    if (frames_.empty()) {
        throw std::logic_error("AnimationClip::frameAt called on empty clip");
    }

    const double elapsedSec = std::max(Kfc::Progress::kMin,
                                        static_cast<double>(elapsedMs) / Kfc::Timing::kMillisecondsPerSecond);
    const int rawIndex = static_cast<int>(std::floor(elapsedSec * framesPerSec_));

    int index = Kfc::Animation::kLoopIndexFloor;
    if (loop_) {
        index = rawIndex % static_cast<int>(frames_.size());
        if (index < Kfc::Animation::kLoopIndexFloor) {
            index = Kfc::Animation::kLoopIndexFloor;
        }
    } else {
        index = std::min(rawIndex, static_cast<int>(frames_.size()) - Kfc::Grid::kForward);
    }

    return frames_[static_cast<std::size_t>(index)];
}
