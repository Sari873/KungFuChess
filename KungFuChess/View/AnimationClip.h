#pragma once
#include "img.h"
#include <string>
#include <vector>

class AnimationClip {
public:
    AnimationClip() = default;
    AnimationClip(std::vector<Img> frames, double framesPerSec, bool loop);

    bool empty() const { return frames_.empty(); }

    const Img& frameAt(long long elapsedMs) const;

private:
    std::vector<Img> frames_;
    double framesPerSec_ = 4.0;
    bool loop_ = true;
};
