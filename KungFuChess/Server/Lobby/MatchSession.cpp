#include "MatchSession.h"
#include "../MatchTickLoop.h"
#include "../TcpFrameSink.h"
#include "../../Adapter/GameEngineSession.h"
#include "../../Engine/GameEngine.h"
#include "../../GameConstants.h"
#include "../../Model/GameState.h"
#include "../../Model/StandardChessSetup.h"
#include "../../Protocol/WireMessage.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace Server {
namespace {

void handleMatchPayload(IGameSession& session,
                        PlayerConnection& player,
                        const std::string& payload) {
    const auto message = Protocol::WireCodec::decode(payload);
    if (!message.has_value()) {
        return;
    }
    if (std::holds_alternative<Protocol::MoveMessage>(*message)) {
        const auto& move = std::get<Protocol::MoveMessage>(*message);
        session.requestMove(move.src, move.dst);
        return;
    }
    if (std::holds_alternative<Protocol::JumpMessage>(*message)) {
        session.requestJump(std::get<Protocol::JumpMessage>(*message).cell);
        return;
    }
    if (std::holds_alternative<Protocol::LegalMovesRequest>(*message)) {
        const auto& req = std::get<Protocol::LegalMovesRequest>(*message);
        Protocol::LegalMovesReply reply;
        reply.src = req.src;
        reply.destinations = session.legalDestinations(req.src);
        sendWire(player, Protocol::WireCodec::encode(reply));
    }
}

bool pumpPlayer(IGameSession& session, PlayerConnection& player) {
    char buffer[4096];
    while (true) {
        const int n = player.socket.recvSome(buffer, sizeof(buffer));
        if (n == 0) {
            return false;
        }
        if (n < 0) {
            break;
        }
        player.framer.append(buffer, n);
    }
    while (auto payload = player.framer.tryPopMessage()) {
        handleMatchPayload(session, player, *payload);
    }
    return true;
}

} 

void MatchSession::run(int matchId,
                       PlayerConnectionPtr white,
                       PlayerConnectionPtr black) {
    if (!white || !black) {
        return;
    }

    white->phase = LobbyPhase::InMatch;
    black->phase = LobbyPhase::InMatch;
    white->socket.setBlocking(false);
    black->socket.setBlocking(false);

    std::cout << "Match " << matchId << " started: "
              << white->name << " (White) vs " << black->name << " (Black)\n";

    GameEngine engine{ GameState(StandardChessSetup::create()) };
    GameEngineSession session(engine);

    std::mutex clientsMutex;
    TcpFrameSink::ClientList clients{ &white->socket, &black->socket };
    TcpFrameSink sink(clientsMutex, clients);
    MatchTickLoop loop(session, sink);

    auto lastTick = std::chrono::steady_clock::now();
    while (true) {
        if (!pumpPlayer(session, *white) || !pumpPlayer(session, *black)) {
            std::cout << "Match " << matchId << " ended (disconnect).\n";
            break;
        }

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count();
        if (elapsed >= Kfc::Server::kTickIntervalMs) {
            loop.tick(Kfc::Server::kTickIntervalMs);
            lastTick = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Kfc::Server::kSocketPollMs));
    }
}

} 
