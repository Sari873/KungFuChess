#pragma once

#include "IFrameSink.h"
#include "../Network/MessageFramer.h"
#include "../Network/TcpSocket.h"
#include "../Protocol/WireMessage.h"
#include <functional>
#include <mutex>
#include <vector>

namespace Server {

/** Broadcasts encoded GameFrames to all connected TCP clients. */
class TcpFrameSink final : public IFrameSink {
public:
    using ClientList = std::vector<Network::TcpSocket*>;

    explicit TcpFrameSink(std::mutex& clientsMutex, ClientList& clients)
        : clientsMutex_(clientsMutex), clients_(clients) {
    }

    void publish(const GameFrame& frame, const std::string& /*encoded*/) override {
        const Protocol::FrameMessage msg{ frame };
        const std::string wire = Protocol::WireCodec::encode(msg);
        Network::MessageFramer framer;

        std::lock_guard<std::mutex> lock(clientsMutex_);
        for (Network::TcpSocket* client : clients_) {
            if (client != nullptr && client->valid()) {
                framer.sendMessage(*client, wire);
            }
        }
    }

private:
    std::mutex& clientsMutex_;
    ClientList& clients_;
};

} 
