#pragma once

#include "../GameConstants.h"
#include "TcpSocket.h"
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace Network {

/** Length-prefixed framing: uint32 LE size + UTF-8 payload. */
class MessageFramer {
public:
    bool sendMessage(TcpSocket& socket, const std::string& payload) {
        if (payload.size() > Kfc::Server::kMaxWirePayloadBytes) {
            return false;
        }
        const std::uint32_t len = static_cast<std::uint32_t>(payload.size());
        char header[4];
        header[0] = static_cast<char>(len & 0xFFu);
        header[1] = static_cast<char>((len >> 8) & 0xFFu);
        header[2] = static_cast<char>((len >> 16) & 0xFFu);
        header[3] = static_cast<char>((len >> 24) & 0xFFu);
        if (!socket.sendAll(header, 4)) {
            return false;
        }
        return socket.sendAll(payload);
    }

    /** Feed bytes from a non-blocking recv; may yield 0..N complete messages. */
    void append(const char* data, int size) {
        buffer_.insert(buffer_.end(), data, data + size);
    }

    std::optional<std::string> tryPopMessage() {
        if (buffer_.size() < 4) {
            return std::nullopt;
        }
        const std::uint32_t len =
            static_cast<std::uint32_t>(static_cast<unsigned char>(buffer_[0]))
            | (static_cast<std::uint32_t>(static_cast<unsigned char>(buffer_[1])) << 8)
            | (static_cast<std::uint32_t>(static_cast<unsigned char>(buffer_[2])) << 16)
            | (static_cast<std::uint32_t>(static_cast<unsigned char>(buffer_[3])) << 24);

        if (len > Kfc::Server::kMaxWirePayloadBytes) {
            buffer_.clear();
            throw std::runtime_error("wire payload too large");
        }
        if (buffer_.size() < 4 + len) {
            return std::nullopt;
        }
        std::string payload(buffer_.begin() + 4, buffer_.begin() + 4 + static_cast<std::ptrdiff_t>(len));
        buffer_.erase(buffer_.begin(), buffer_.begin() + 4 + static_cast<std::ptrdiff_t>(len));
        return payload;
    }

    void clear() { buffer_.clear(); }

private:
    std::vector<char> buffer_;
};

} 
