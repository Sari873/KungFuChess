#include "TcpSocket.h"
#include <cstring>

namespace Network {

namespace {

sockaddr_in makeAddr(unsigned short port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    return addr;
}

} 

void TcpSocket::listenOn(unsigned short port, int backlog) {
    close();
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        throw std::runtime_error("socket() failed");
    }

    BOOL reuse = TRUE;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    const sockaddr_in addr = makeAddr(port);
    if (bind(socket_, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        close();
        throw std::runtime_error("bind() failed");
    }
    if (listen(socket_, backlog) == SOCKET_ERROR) {
        close();
        throw std::runtime_error("listen() failed");
    }
}

TcpSocket TcpSocket::acceptClient() {
    const SOCKET client = ::accept(socket_, nullptr, nullptr);
    if (client == INVALID_SOCKET) {
        throw std::runtime_error("accept() failed");
    }
    return TcpSocket(client);
}

std::optional<TcpSocket> TcpSocket::tryAccept() {
    const SOCKET client = ::accept(socket_, nullptr, nullptr);
    if (client == INVALID_SOCKET) {
        const int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            return std::nullopt;
        }
        throw std::runtime_error("accept() failed: " + std::to_string(err));
    }
    return TcpSocket(client);
}

void TcpSocket::connectTo(const std::string& host, unsigned short port) {
    close();
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        throw std::runtime_error("socket() failed");
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* result = nullptr;
    const std::string portText = std::to_string(port);
    if (getaddrinfo(host.c_str(), portText.c_str(), &hints, &result) != 0 || result == nullptr) {
        close();
        throw std::runtime_error("getaddrinfo failed for " + host);
    }

    const int rc = ::connect(socket_, result->ai_addr, static_cast<int>(result->ai_addrlen));
    freeaddrinfo(result);
    if (rc == SOCKET_ERROR) {
        close();
        throw std::runtime_error("connect() failed to " + host + ":" + portText);
    }
}

void TcpSocket::setBlocking(bool blocking) {
    u_long mode = blocking ? 0UL : 1UL;
    if (ioctlsocket(socket_, FIONBIO, &mode) == SOCKET_ERROR) {
        throw std::runtime_error("ioctlsocket(FIONBIO) failed");
    }
}

bool TcpSocket::sendAll(const char* data, int size) {
    int sent = 0;
    while (sent < size) {
        const int n = ::send(socket_, data + sent, size - sent, 0);
        if (n == SOCKET_ERROR) {
            return false;
        }
        sent += n;
    }
    return true;
}

int TcpSocket::recvSome(char* buffer, int capacity) {
    const int n = ::recv(socket_, buffer, capacity, 0);
    if (n == 0) {
        return 0;
    }
    if (n == SOCKET_ERROR) {
        const int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            return -1;
        }
        throw std::runtime_error("recv() failed: " + std::to_string(err));
    }
    return n;
}

} 
