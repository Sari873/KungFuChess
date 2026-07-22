#pragma once

#include "../Presentation/IGameSession.h"
#include "IFrameSink.h"

namespace Server {

/**
 * Authoritative match clock: advance session time, capture GameFrame, encode, publish.
 * Networking will later replace StdoutFrameSink with a TCP broadcaster.
 */
class MatchTickLoop {
public:
    MatchTickLoop(IGameSession& session, IFrameSink& sink);

    /** One tick: advanceTime → capture → encode → sink.publish. Returns encoded payload. */
    std::string tick(long long deltaMs);

    /** Run fixed ticks sleeping between them (Step 2 demo / soak). */
    void runFixedTicks(int tickCount, int intervalMs);

private:
    IGameSession& session_;
    IFrameSink& sink_;
};

} 
