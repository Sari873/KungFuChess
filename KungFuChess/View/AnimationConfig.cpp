#include "AnimationConfig.h"
#include <cctype>
#include <fstream>
#include <sstream>

namespace {

bool readFile(const std::string& path, std::string& out) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

bool parseDoubleAfterKey(const std::string& text, const std::string& key, double& out) {
    const auto keyPos = text.find(key);
    if (keyPos == std::string::npos) {
        return false;
    }
    const auto colon = text.find(':', keyPos + key.size());
    if (colon == std::string::npos) {
        return false;
    }

    std::size_t i = colon + 1;
    while (i < text.size() && std::isspace(static_cast<unsigned char>(text[i]))) {
        ++i;
    }

    std::size_t j = i;
    while (j < text.size() &&
           (std::isdigit(static_cast<unsigned char>(text[j])) || text[j] == '.' || text[j] == '-')) {
        ++j;
    }
    if (j == i) {
        return false;
    }

    out = std::stod(text.substr(i, j - i));
    return true;
}

bool parseBoolAfterKey(const std::string& text, const std::string& key, bool& out) {
    const auto keyPos = text.find(key);
    if (keyPos == std::string::npos) {
        return false;
    }
    const auto colon = text.find(':', keyPos + key.size());
    if (colon == std::string::npos) {
        return false;
    }

    const auto truePos = text.find("true", colon);
    const auto falsePos = text.find("false", colon);
    if (truePos != std::string::npos && (falsePos == std::string::npos || truePos < falsePos)) {
        out = true;
        return true;
    }
    if (falsePos != std::string::npos) {
        out = false;
        return true;
    }
    return false;
}

} 

AnimationConfig AnimationConfig::defaultsForState(const std::string& stateFolder) {
    AnimationConfig cfg;
    if (stateFolder == "move") {
        cfg.framesPerSec = 8.0;
        cfg.loop = true;
    } else if (stateFolder == "jump") {
        cfg.framesPerSec = 10.0;
        cfg.loop = false;
    } else {
        cfg.framesPerSec = 4.0;
        cfg.loop = true;
    }
    return cfg;
}

AnimationConfig AnimationConfig::loadOrDefault(const std::string& configPath, const AnimationConfig& fallback) {
    std::string text;
    if (!readFile(configPath, text)) {
        return fallback;
    }

    AnimationConfig cfg = fallback;
    double fps = fallback.framesPerSec;
    bool loop = fallback.loop;
    if (parseDoubleAfterKey(text, "frames_per_sec", fps)) {
        cfg.framesPerSec = fps;
    }
    if (parseBoolAfterKey(text, "is_loop", loop)) {
        cfg.loop = loop;
    }
    return cfg;
}
