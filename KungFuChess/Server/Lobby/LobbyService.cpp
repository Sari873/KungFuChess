#include "LobbyService.h"
#include "MatchSession.h"
#include "Matchmaker.h"
#include "RoomRegistry.h"
#include "../../GameConstants.h"
#include "../../Network/TcpSocket.h"
#include "../../Protocol/WireMessage.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace Server {
namespace {

void sendError(PlayerConnection& player, const std::string& reason) {
    sendWire(player, Protocol::WireCodec::encode(Protocol::LobbyErrorMessage{ reason }));
}

void assignMatchmakingColors(PlayerConnectionPtr a,
                             PlayerConnectionPtr b,
                             PlayerConnectionPtr& white,
                             PlayerConnectionPtr& black) {
    if (a->rating > b->rating) {
        white = a;
        black = b;
    } else if (b->rating > a->rating) {
        white = b;
        black = a;
    } else {
        white = a;
        black = b;
    }
}

void startMatch(int matchId,
                PlayerConnectionPtr white,
                PlayerConnectionPtr black,
                std::vector<std::thread>& matchThreads) {
    Protocol::MatchReadyMessage readyWhite;
    readyWhite.matchId = matchId;
    readyWhite.yourColor = DisplayColor::White;
    readyWhite.opponentName = black->name;
    readyWhite.opponentRating = black->rating;

    Protocol::MatchReadyMessage readyBlack;
    readyBlack.matchId = matchId;
    readyBlack.yourColor = DisplayColor::Black;
    readyBlack.opponentName = white->name;
    readyBlack.opponentRating = white->rating;

    sendWire(*white, Protocol::WireCodec::encode(readyWhite));
    sendWire(*black, Protocol::WireCodec::encode(readyBlack));

    matchThreads.emplace_back([matchId, white, black]() {
        MatchSession::run(matchId, white, black);
    });
}

void handleLobbyMessage(PlayerConnectionPtr player,
                        const Protocol::WireMessage& message,
                        Matchmaker& matchmaker,
                        RoomRegistry& rooms,
                        int& nextMatchId,
                        std::vector<std::thread>& matchThreads) {
    if (std::holds_alternative<Protocol::HelloMessage>(message)) {
        const auto& hello = std::get<Protocol::HelloMessage>(message);
        player->name = hello.name.empty() ? ("Player" + std::to_string(player->playerId)) : hello.name;
        player->rating = hello.rating > 0 ? hello.rating : Kfc::Server::kDefaultPlayerRating;
        player->phase = LobbyPhase::Idle;
        sendWire(*player, Protocol::WireCodec::encode(Protocol::WelcomeMessage{ player->playerId }));
        return;
    }

    if (player->phase == LobbyPhase::NeedHello) {
        sendError(*player, "send_hello_first");
        return;
    }

    if (std::holds_alternative<Protocol::FindMatchMessage>(message)) {
        if (player->phase == LobbyPhase::Queued || player->phase == LobbyPhase::InRoom) {
            sendError(*player, "already_busy");
            return;
        }
        player->phase = LobbyPhase::Queued;
        matchmaker.enqueue(player);
        sendWire(*player,
                 Protocol::WireCodec::encode(
                     Protocol::QueueStatusMessage{ matchmaker.positionOf(player->playerId) }));
        return;
    }

    if (std::holds_alternative<Protocol::CancelFindMessage>(message)) {
        matchmaker.dequeue(player->playerId);
        if (player->phase == LobbyPhase::Queued) {
            player->phase = LobbyPhase::Idle;
        }
        return;
    }

    if (std::holds_alternative<Protocol::CreateRoomMessage>(message)) {
        if (player->phase != LobbyPhase::Idle) {
            sendError(*player, "already_busy");
            return;
        }
        matchmaker.dequeue(player->playerId);
        const std::string code = rooms.createRoom(player);
        player->phase = LobbyPhase::InRoom;
        player->roomCode = code;
        sendWire(*player, Protocol::WireCodec::encode(Protocol::RoomCreatedMessage{ code }));
        sendWire(*player, Protocol::WireCodec::encode(Protocol::RoomWaitingMessage{ code, 1 }));
        return;
    }

    if (std::holds_alternative<Protocol::JoinRoomMessage>(message)) {
        if (player->phase != LobbyPhase::Idle) {
            sendError(*player, "already_busy");
            return;
        }
        const auto& join = std::get<Protocol::JoinRoomMessage>(message);
        Room completed;
        if (auto err = rooms.tryJoin(join.code, player, completed)) {
            sendError(*player, *err);
            return;
        }
        // Creator = White, joiner = Black.
        player->phase = LobbyPhase::InMatch;
        completed.creator->phase = LobbyPhase::InMatch;
        startMatch(nextMatchId++, completed.creator, completed.joiner, matchThreads);
        return;
    }
}

bool pumpLobbyPlayer(PlayerConnectionPtr player,
                     Matchmaker& matchmaker,
                     RoomRegistry& rooms,
                     int& nextMatchId,
                     std::vector<std::thread>& matchThreads) {
    char buffer[4096];
    while (true) {
        const int n = player->socket.recvSome(buffer, sizeof(buffer));
        if (n == 0) {
            return false;
        }
        if (n < 0) {
            break;
        }
        player->framer.append(buffer, n);
    }

    while (auto payload = player->framer.tryPopMessage()) {
        const auto message = Protocol::WireCodec::decode(*payload);
        if (!message) {
            sendError(*player, "malformed_message");
            continue;
        }
        handleLobbyMessage(player, *message, matchmaker, rooms, nextMatchId, matchThreads);
    }
    return true;
}

} 

LobbyService::LobbyService(unsigned short port) : port_(port) {}

void LobbyService::run() {
    Network::WinsockContext winsock;
    Network::TcpSocket listener;
    listener.listenOn(port_);
    listener.setBlocking(false);

    std::cout << "KungFuChess lobby listening on port " << port_ << "...\n";

    Matchmaker matchmaker;
    RoomRegistry rooms;
    std::vector<PlayerConnectionPtr> lobbyPlayers;
    std::vector<std::thread> matchThreads;
    int nextPlayerId = 1;
    int nextMatchId = 1;

    while (true) {
        if (auto accepted = listener.tryAccept()) {
            auto player = std::make_shared<PlayerConnection>();
            player->socket = std::move(*accepted);
            player->socket.setBlocking(false);
            player->playerId = nextPlayerId++;
            player->rating = Kfc::Server::kDefaultPlayerRating;
            lobbyPlayers.push_back(player);
            std::cout << "Lobby client connected id=" << player->playerId << "\n";
        }

        for (auto it = lobbyPlayers.begin(); it != lobbyPlayers.end();) {
            PlayerConnectionPtr player = *it;
            if (player->phase == LobbyPhase::InMatch) {
                it = lobbyPlayers.erase(it);
                continue;
            }
            if (!pumpLobbyPlayer(player, matchmaker, rooms, nextMatchId, matchThreads)) {
                std::cout << "Lobby client disconnected id=" << player->playerId << "\n";
                matchmaker.dequeue(player->playerId);
                rooms.leave(player->playerId);
                it = lobbyPlayers.erase(it);
                continue;
            }
            ++it;
        }

        PlayerConnectionPtr a;
        PlayerConnectionPtr b;
        while (matchmaker.tryPopPair(a, b)) {
            a->phase = LobbyPhase::InMatch;
            b->phase = LobbyPhase::InMatch;
            PlayerConnectionPtr white;
            PlayerConnectionPtr black;
            assignMatchmakingColors(a, b, white, black);
            startMatch(nextMatchId++, white, black, matchThreads);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(Kfc::Server::kSocketPollMs));
    }
}

} 
