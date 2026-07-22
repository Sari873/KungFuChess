#pragma once

#include "../../Network/MessageFramer.h"
#include "../../Network/TcpSocket.h"
#include <memory>
#include <string>

namespace Server {

enum class LobbyPhase {
    NeedHello,
    Idle,
    Queued,
    InRoom,
    InMatch
};

struct PlayerConnection {
    Network::TcpSocket socket;
    Network::MessageFramer framer;
    int playerId = 0;
    std::string name = "Player";
    int rating = 1000;
    LobbyPhase phase = LobbyPhase::NeedHello;
    std::string roomCode;
};

using PlayerConnectionPtr = std::shared_ptr<PlayerConnection>;

inline bool sendWire(PlayerConnection& player, const std::string& payload) {
    Network::MessageFramer outbound;
    return outbound.sendMessage(player.socket, payload);
}

} 
