#include "AnimationClip.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

AnimationClip::AnimationClip(std::vector<Img> frames, double framesPerSec, bool loop)
    : frames_(std::move(frames))
    , framesPerSec_(framesPerSec > 0.0 ? framesPerSec : 4.0)
    , loop_(loop) {
}

const Img& AnimationClip::frameAt(long long elapsedMs) const {
    if (frames_.empty()) {
        throw std::logic_error("AnimationClip::frameAt called on empty clip");
    }

    const double elapsedSec = std::max(0.0, static_cast<double>(elapsedMs) / 1000.0);
    const int rawIndex = static_cast<int>(std::floor(elapsedSec * framesPerSec_));

    int index = 0;
    if (loop_) {
        index = rawIndex % static_cast<int>(frames_.size());
        if (index < 0) {
            index = 0;
        }
    } else {
        index = std::min(rawIndex, static_cast<int>(frames_.size()) - 1);
    }

    return frames_[static_cast<std::size_t>(index)];
}
