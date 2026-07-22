#pragma once

#include "NetworkGameSession.h"
#include <memory>
#include <string>

class LobbyClient {
public:
    LobbyClient(std::string host, unsigned short port);

    /** Console menu: Play / Create room / Join room. Returns a ready game session. */
    std::unique_ptr<NetworkGameSession> runMenu(const std::string& playerName, int rating);

private:
    void connectAndHello(const std::string& playerName, int rating);
    void send(const Protocol::WireMessage& message);
    bool waitForMatchReady(Protocol::MatchReadyMessage& out);
    std::unique_ptr<NetworkGameSession> takeSession(const Protocol::MatchReadyMessage& ready);

    std::string host_;
    unsigned short port_;
    Network::WinsockContext winsock_;
    Network::TcpSocket socket_;
    Network::MessageFramer framer_;
    bool helloDone_ = false;
};
