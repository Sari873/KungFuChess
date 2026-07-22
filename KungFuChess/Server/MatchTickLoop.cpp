#include "MatchTickLoop.h"
#include "../Presentation/GameFrame.h"
#include "../Protocol/GameFrameCodec.h"
#include <chrono>
#include <stdexcept>
#include <thread>

namespace Server {

MatchTickLoop::MatchTickLoop(IGameSession& session, IFrameSink& sink)
    : session_(session), sink_(sink) {
}

std::string MatchTickLoop::tick(long long deltaMs) {
    session_.advanceTime(deltaMs);
    const GameFrame frame = captureGameFrame(session_);
    const std::string encoded = Protocol::GameFrameCodec::encode(frame);
    sink_.publish(frame, encoded);
    return encoded;
}

void MatchTickLoop::runFixedTicks(int tickCount, int intervalMs) {
    if (tickCount < 0 || intervalMs < 0) {
        throw std::invalid_argument("MatchTickLoop::runFixedTicks invalid args");
    }
    for (int i = 0; i < tickCount; ++i) {
        tick(intervalMs);
        if (i + 1 < tickCount) {
            std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        }
    }
}

} 
