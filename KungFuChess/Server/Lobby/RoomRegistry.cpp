#include "RoomRegistry.h"
#include "../../GameConstants.h"
#include <chrono>
#include <random>

namespace Server {

std::string RoomRegistry::generateCode() const {
    static constexpr char kAlphabet[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    static thread_local std::mt19937 rng{
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
    std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(kAlphabet) - 2));
    std::string code;
    code.resize(static_cast<std::size_t>(Kfc::Server::kRoomCodeLength));
    for (int i = 0; i < Kfc::Server::kRoomCodeLength; ++i) {
        code[static_cast<std::size_t>(i)] = kAlphabet[dist(rng)];
    }
    return code;
}

std::string RoomRegistry::createRoom(PlayerConnectionPtr creator) {
    std::string code;
    do {
        code = generateCode();
    } while (rooms_.count(code) != 0);

    Room room;
    room.code = code;
    room.creator = std::move(creator);
    rooms_[code] = room;
    return code;
}

std::optional<std::string> RoomRegistry::tryJoin(const std::string& code,
                                                 PlayerConnectionPtr joiner,
                                                 Room& completedRoom) {
    const auto it = rooms_.find(code);
    if (it == rooms_.end()) {
        return std::string("room_not_found");
    }
    if (it->second.joiner) {
        return std::string("room_full");
    }
    if (it->second.creator && it->second.creator->playerId == joiner->playerId) {
        return std::string("already_in_room");
    }
    it->second.joiner = std::move(joiner);
    completedRoom = it->second;
    rooms_.erase(it);
    return std::nullopt;
}

void RoomRegistry::leave(int playerId) {
    for (auto it = rooms_.begin(); it != rooms_.end();) {
        Room& room = it->second;
        if ((room.creator && room.creator->playerId == playerId)
            || (room.joiner && room.joiner->playerId == playerId)) {
            it = rooms_.erase(it);
        } else {
            ++it;
        }
    }
}

int RoomRegistry::playerCount(const std::string& code) const {
    const auto it = rooms_.find(code);
    if (it == rooms_.end()) {
        return 0;
    }
    int n = 0;
    if (it->second.creator) ++n;
    if (it->second.joiner) ++n;
    return n;
}

} 
