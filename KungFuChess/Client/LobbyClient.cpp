#include "LobbyClient.h"
#include "../GameConstants.h"
#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <thread>

LobbyClient::LobbyClient(std::string host, unsigned short port)
    : host_(std::move(host)), port_(port) {
}

void LobbyClient::send(const Protocol::WireMessage& message) {
    Network::MessageFramer outbound;
    if (!outbound.sendMessage(socket_, Protocol::WireCodec::encode(message))) {
        throw std::runtime_error("failed to send lobby message");
    }
}

void LobbyClient::connectAndHello(const std::string& playerName, int rating) {
    socket_.connectTo(host_, port_);
    socket_.setBlocking(false);
    send(Protocol::HelloMessage{ playerName, rating });
    helloDone_ = true;

    // Wait for Welcome.
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (std::chrono::steady_clock::now() < deadline) {
        char buffer[4096];
        const int n = socket_.recvSome(buffer, sizeof(buffer));
        if (n > 0) {
            framer_.append(buffer, n);
        } else if (n == 0) {
            throw std::runtime_error("server closed during hello");
        }
        while (auto payload = framer_.tryPopMessage()) {
            const auto msg = Protocol::WireCodec::decode(*payload);
            if (msg && std::holds_alternative<Protocol::WelcomeMessage>(*msg)) {
                std::cout << "Lobby welcome, playerId="
                          << std::get<Protocol::WelcomeMessage>(*msg).playerId << "\n";
                return;
            }
            if (msg && std::holds_alternative<Protocol::LobbyErrorMessage>(*msg)) {
                throw std::runtime_error(std::get<Protocol::LobbyErrorMessage>(*msg).reason);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Kfc::Server::kSocketPollMs));
    }
    throw std::runtime_error("timed out waiting for Welcome");
}

bool LobbyClient::waitForMatchReady(Protocol::MatchReadyMessage& out) {
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::minutes(10);
    while (std::chrono::steady_clock::now() < deadline) {
        char buffer[4096];
        const int n = socket_.recvSome(buffer, sizeof(buffer));
        if (n > 0) {
            framer_.append(buffer, n);
        } else if (n == 0) {
            throw std::runtime_error("server closed while waiting for match");
        }

        while (auto payload = framer_.tryPopMessage()) {
            const auto msg = Protocol::WireCodec::decode(*payload);
            if (!msg) {
                continue;
            }
            if (std::holds_alternative<Protocol::QueueStatusMessage>(*msg)) {
                std::cout << "Queue position: "
                          << std::get<Protocol::QueueStatusMessage>(*msg).position << "\n";
            } else if (std::holds_alternative<Protocol::RoomCreatedMessage>(*msg)) {
                std::cout << "Room created. Code: "
                          << std::get<Protocol::RoomCreatedMessage>(*msg).code << "\n";
            } else if (std::holds_alternative<Protocol::RoomWaitingMessage>(*msg)) {
                const auto& w = std::get<Protocol::RoomWaitingMessage>(*msg);
                std::cout << "Room " << w.code << " players=" << w.players << "\n";
            } else if (std::holds_alternative<Protocol::MatchReadyMessage>(*msg)) {
                out = std::get<Protocol::MatchReadyMessage>(*msg);
                return true;
            } else if (std::holds_alternative<Protocol::LobbyErrorMessage>(*msg)) {
                throw std::runtime_error(std::get<Protocol::LobbyErrorMessage>(*msg).reason);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Kfc::Server::kSocketPollMs));
    }
    return false;
}

std::unique_ptr<NetworkGameSession> LobbyClient::takeSession(
    const Protocol::MatchReadyMessage& ready) {
    return std::make_unique<NetworkGameSession>(std::move(winsock_),
                                                std::move(socket_),
                                                std::move(framer_),
                                                ready.yourColor,
                                                ready);
}

std::unique_ptr<NetworkGameSession> LobbyClient::runMenu(const std::string& playerName,
                                                        int rating) {
    connectAndHello(playerName, rating);

    std::cout << "\n=== KungFuChess Lobby ===\n"
              << "1) Play (matchmaking ±" << Kfc::Server::kRatingMatchWindow << ")\n"
              << "2) Create room\n"
              << "3) Join room\n"
              << "Choice: ";

    int choice = 0;
    std::cin >> choice;
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    if (choice == 1) {
        std::cout << "Searching for opponent...\n";
        send(Protocol::FindMatchMessage{});
    } else if (choice == 2) {
        send(Protocol::CreateRoomMessage{});
        std::cout << "Waiting for opponent to join your room...\n";
    } else if (choice == 3) {
        std::cout << "Enter room code: ";
        std::string code;
        std::getline(std::cin, code);
        send(Protocol::JoinRoomMessage{ code });
        std::cout << "Joining room " << code << "...\n";
    } else {
        throw std::runtime_error("invalid lobby choice");
    }

    Protocol::MatchReadyMessage ready;
    if (!waitForMatchReady(ready)) {
        throw std::runtime_error("timed out waiting for MatchReady");
    }

    std::cout << "Match ready! You are "
              << Protocol::colorToWire(ready.yourColor)
              << " vs " << ready.opponentName
              << " (rating " << ready.opponentRating << ")\n";

    return takeSession(ready);
}
