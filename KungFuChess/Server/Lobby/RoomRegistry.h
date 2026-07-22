#pragma once

#include "PlayerConnection.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Server {

struct Room {
    std::string code;
    PlayerConnectionPtr creator;
    PlayerConnectionPtr joiner;
};

class RoomRegistry {
public:
    std::string createRoom(PlayerConnectionPtr creator);
    /** Returns error reason, or nullopt on success. When full, outJoiner is set and room is removed. */
    std::optional<std::string> tryJoin(const std::string& code,
                                       PlayerConnectionPtr joiner,
                                       Room& completedRoom);
    void leave(int playerId);
    int playerCount(const std::string& code) const;

private:
    std::string generateCode() const;
    std::unordered_map<std::string, Room> rooms_;
};

} 
