#pragma once

#include "PlayerConnection.h"
#include <vector>

namespace Server {

class Matchmaker {
public:
    void enqueue(PlayerConnectionPtr player);
    void dequeue(int playerId);
    int positionOf(int playerId) const;

    /** If two compatible players are waiting, remove and return them (first=queue order). */
    bool tryPopPair(PlayerConnectionPtr& a, PlayerConnectionPtr& b);

private:
    std::vector<PlayerConnectionPtr> queue_;
};

} 
