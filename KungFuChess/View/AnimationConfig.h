#pragma once
#include "../GameConstants.h"
#include <string>

struct AnimationConfig {
    double framesPerSec = Kfc::Animation::kDefaultFramesPerSec;
    bool loop = true;

    static AnimationConfig defaultsForState(const std::string& stateFolder);
    static AnimationConfig loadOrDefault(const std::string& configPath, const AnimationConfig& fallback);
};
