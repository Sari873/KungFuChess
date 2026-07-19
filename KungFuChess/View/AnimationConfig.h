#pragma once
#include <string>

struct AnimationConfig {
    double framesPerSec = 4.0;
    bool loop = true;

    static AnimationConfig defaultsForState(const std::string& stateFolder);
    static AnimationConfig loadOrDefault(const std::string& configPath, const AnimationConfig& fallback);
};
