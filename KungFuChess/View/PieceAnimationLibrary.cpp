#include "PieceAnimationLibrary.h"
#include "AnimationConfig.h"
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

PieceAnimationLibrary::PieceAnimationLibrary(std::string assetsRoot)
    : assetsRoot_(std::move(assetsRoot)) {
}

const AnimationClip& PieceAnimationLibrary::clip(const std::string& notation,
                                                 const std::string& stateFolder,
                                                 int cellSizePx) {
    const std::string key = notation + "|" + stateFolder + "|" + std::to_string(cellSizePx);
    const auto found = cache_.find(key);
    if (found != cache_.end()) {
        return found->second;
    }

    auto [it, inserted] = cache_.emplace(key, loadClip(notation, stateFolder, cellSizePx));
    (void)inserted;
    return it->second;
}

AnimationClip PieceAnimationLibrary::loadClip(const std::string& notation,
                                              const std::string& stateFolder,
                                              int cellSizePx) const {
    const fs::path stateDir = fs::path(assetsRoot_) / notation / "states" / stateFolder;
    const fs::path spritesDir = stateDir / "sprites";
    const fs::path configPath = stateDir / "config.json";

    const AnimationConfig fallback = AnimationConfig::defaultsForState(stateFolder);
    const AnimationConfig cfg = AnimationConfig::loadOrDefault(configPath.string(), fallback);

    std::vector<Img> frames;
    frames.reserve(8);

    for (int i = 1; i <= 64; ++i) {
        const fs::path framePath = spritesDir / (std::to_string(i) + ".png");
        if (!fs::exists(framePath)) {
            break;
        }

        Img frame;
        try {
            frame.read(framePath.string(), { cellSizePx, cellSizePx }, true, cv::INTER_AREA);
        }
        catch (const std::runtime_error&) {
            break;
        }
        frames.push_back(std::move(frame));
    }

    return AnimationClip(std::move(frames), cfg.framesPerSec, cfg.loop);
}
