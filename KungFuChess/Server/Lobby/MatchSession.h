#pragma once

#include "PlayerConnection.h"

namespace Server {

/** Runs an authoritative two-player match until a client disconnects. */
class MatchSession {
public:
    static void run(int matchId,
                    PlayerConnectionPtr white,
                    PlayerConnectionPtr black);
};

} 
