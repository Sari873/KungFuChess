#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <optional>
#include <stdexcept>
#include <string>

#pragma comment(lib, "ws2_32.lib")

namespace Network {

class WinsockContext {
public:
    WinsockContext() {
        WSADATA data{};
        const int rc = WSAStartup(MAKEWORD(2, 2), &data);
        if (rc != 0) {
            throw std::runtime_error("WSAStartup failed: " + std::to_string(rc));
        }
        active_ = true;
    }

    ~WinsockContext() {
        if (active_) {
            WSACleanup();
        }
    }

    WinsockContext(const WinsockContext&) = delete;
    WinsockContext& operator=(const WinsockContext&) = delete;

    WinsockContext(WinsockContext&& other) noexcept : active_(other.active_) {
        other.active_ = false;
    }

    WinsockContext& operator=(WinsockContext&& other) noexcept {
        if (this != &other) {
            if (active_) {
                WSACleanup();
            }
            active_ = other.active_;
            other.active_ = false;
        }
        return *this;
    }

private:
    bool active_ = false;
};

class TcpSocket {
public:
    TcpSocket() = default;
    explicit TcpSocket(SOCKET s) : socket_(s) {}

    ~TcpSocket() { close(); }

    TcpSocket(TcpSocket&& other) noexcept : socket_(other.socket_) {
        other.socket_ = INVALID_SOCKET;
    }

    TcpSocket& operator=(TcpSocket&& other) noexcept {
        if (this != &other) {
            close();
            socket_ = other.socket_;
            other.socket_ = INVALID_SOCKET;
        }
        return *this;
    }

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    bool valid() const { return socket_ != INVALID_SOCKET; }
    SOCKET handle() const { return socket_; }

    void close() {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
    }

    void listenOn(unsigned short port, int backlog = 4);
    TcpSocket acceptClient();
    /** Non-blocking accept; empty if no pending connection. */
    std::optional<TcpSocket> tryAccept();
    void connectTo(const std::string& host, unsigned short port);

    void setBlocking(bool blocking);
    bool sendAll(const char* data, int size);
    bool sendAll(const std::string& data) {
        return sendAll(data.data(), static_cast<int>(data.size()));
    }

    /** Returns bytes read, 0 on graceful close, -1 on would-block, throws on hard error. */
    int recvSome(char* buffer, int capacity);

private:
    SOCKET socket_ = INVALID_SOCKET;
};

} 
