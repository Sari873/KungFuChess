#include "TcpMatchHost.h"
#include "MatchTickLoop.h"
#include "TcpFrameSink.h"
#include "../GameConstants.h"
#include "../Network/MessageFramer.h"
#include "../Network/TcpSocket.h"
#include "../Protocol/WireMessage.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace Server {
namespace {

struct ClientConnection {
    Network::TcpSocket socket;
    Network::MessageFramer framer;
};

void handlePayload(IGameSession& session,
                   Network::TcpSocket& socket,
                   Network::MessageFramer& sendFramer,
                   const std::string& payload) {
    const auto message = Protocol::WireCodec::decode(payload);
    if (!message.has_value()) {
        std::cerr << "Ignoring malformed wire message\n";
        return;
    }

    if (std::holds_alternative<Protocol::MoveMessage>(*message)) {
        const auto& move = std::get<Protocol::MoveMessage>(*message);
        session.requestMove(move.src, move.dst);
        return;
    }
    if (std::holds_alternative<Protocol::JumpMessage>(*message)) {
        const auto& jump = std::get<Protocol::JumpMessage>(*message);
        session.requestJump(jump.cell);
        return;
    }
    if (std::holds_alternative<Protocol::LegalMovesRequest>(*message)) {
        const auto& req = std::get<Protocol::LegalMovesRequest>(*message);
        Protocol::LegalMovesReply reply;
        reply.src = req.src;
        reply.destinations = session.legalDestinations(req.src);
        sendFramer.sendMessage(socket, Protocol::WireCodec::encode(reply));
        return;
    }
}

bool pumpClient(IGameSession& session, ClientConnection& client) {
    char buffer[4096];
    while (true) {
        const int n = client.socket.recvSome(buffer, sizeof(buffer));
        if (n == 0) {
            return false;
        }
        if (n < 0) {
            break;
        }
        client.framer.append(buffer, n);
    }

    Network::MessageFramer outbound;
    while (auto payload = client.framer.tryPopMessage()) {
        handlePayload(session, client.socket, outbound, *payload);
    }
    return true;
}

} 

TcpMatchHost::TcpMatchHost(unsigned short port) : port_(port) {}

void TcpMatchHost::run(IGameSession& session) {
    Network::WinsockContext winsock;
    Network::TcpSocket listener;
    listener.listenOn(port_);
    std::cout << "KungFuChess server listening on port " << port_ << "...\n";

    ClientConnection client;
    client.socket = listener.acceptClient();
    client.socket.setBlocking(false);
    std::cout << "Client connected.\n";

    Network::MessageFramer welcomeFramer;
    welcomeFramer.sendMessage(client.socket,
                              Protocol::WireCodec::encode(Protocol::WelcomeMessage{ 1 }));

    std::mutex clientsMutex;
    TcpFrameSink::ClientList clientPtrs{ &client.socket };
    TcpFrameSink sink(clientsMutex, clientPtrs);
    MatchTickLoop loop(session, sink);

    auto lastTick = std::chrono::steady_clock::now();
    while (true) {
        if (!pumpClient(session, client)) {
            std::cout << "Client disconnected.\n";
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
