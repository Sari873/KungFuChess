#pragma once
#include "AnimationClip.h"
#include <string>
#include <unordered_map>


class PieceAnimationLibrary {
public:
    explicit PieceAnimationLibrary(std::string assetsRoot);

    const AnimationClip& clip(const std::string& notation,
                              const std::string& stateFolder,
                              int cellSizePx);

private:
    AnimationClip loadClip(const std::string& notation,
                           const std::string& stateFolder,
                           int cellSizePx) const;

    std::string assetsRoot_;
    std::unordered_map<std::string, AnimationClip> cache_;
};
