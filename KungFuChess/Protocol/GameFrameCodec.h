#pragma once

#include "../Presentation/GameFrame.h"
#include <optional>
#include <string>

namespace Protocol {

/** JSON codec for authoritative GameFrame snapshots (shared by server and client). */
class GameFrameCodec {
public:
    static std::string encode(const GameFrame& frame);
    static std::optional<GameFrame> decode(const std::string& json);
};

/** Structural equality used by the server self-test (encode → decode). */
bool gameFramesEqual(const GameFrame& a, const GameFrame& b);

} 
