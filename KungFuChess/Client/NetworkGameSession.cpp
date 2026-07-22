#include "NetworkGameSession.h"
#include "../GameConstants.h"
#include <chrono>
#include <stdexcept>
#include <thread>

NetworkGameSession::NetworkGameSession(std::string host, unsigned short port)
    : winsock_(), socket_() {
    socket_.connectTo(host, port);
    socket_.setBlocking(false);
}

NetworkGameSession::NetworkGameSession(Network::WinsockContext winsock,
                                       Network::TcpSocket socket,
                                       Network::MessageFramer framer,
                                       DisplayColor yourColor,
                                       Protocol::MatchReadyMessage matchInfo)
    : winsock_(std::move(winsock))
    , socket_(std::move(socket))
    , framer_(std::move(framer))
    , yourColor_(yourColor)
    , matchInfo_(std::move(matchInfo)) {
    socket_.setBlocking(false);
}

bool NetworkGameSession::hasFrame() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return hasFrame_;
}

void NetworkGameSession::pollIncoming() const {
    char buffer[8192];
    while (true) {
        const int n = socket_.recvSome(buffer, sizeof(buffer));
        if (n == 0) {
            throw std::runtime_error("server closed the connection");
        }
        if (n < 0) {
            break;
        }
        framer_.append(buffer, n);
    }

    while (auto payload = framer_.tryPopMessage()) {
        applyMessage(*payload);
    }
}

void NetworkGameSession::applyMessage(const std::string& payload) const {
    const auto message = Protocol::WireCodec::decode(payload);
    if (!message.has_value()) {
        return;
    }

    if (std::holds_alternative<Protocol::FrameMessage>(*message)) {
        frame_ = std::get<Protocol::FrameMessage>(*message).frame;
        hasFrame_ = true;
        return;
    }
    if (std::holds_alternative<Protocol::LegalMovesReply>(*message)) {
        pendingLegal_ = std::get<Protocol::LegalMovesReply>(*message);
        return;
    }
}

void NetworkGameSession::advanceTime(long long /*deltaMs*/) {
    std::lock_guard<std::mutex> lock(mutex_);
    pollIncoming();
}

long long NetworkGameSession::clockMs() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.clockMs;
}

bool NetworkGameSession::isGameOver() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.gameOver;
}

std::optional<DisplayColor> NetworkGameSession::winner() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.winner;
}

DisplayBoard NetworkGameSession::board() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.board;
}

bool NetworkGameSession::isEmpty(const CellCoord& cell) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.board.isEmpty(cell);
}

bool NetworkGameSession::isMoving(const CellCoord& cell) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const DisplayMotion& motion : frame_.motions) {
        if (motion.src == cell || motion.dst == cell) {
            return true;
        }
    }
    const DisplayPiece* piece = frame_.board.pieceAt(cell);
    return piece != nullptr && piece->state == DisplayPieceState::Moving;
}

bool NetworkGameSession::isJumping(const CellCoord& cell) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const DisplayJump& jump : frame_.jumps) {
        if (jump.src == cell || jump.dst == cell) {
            return true;
        }
    }
    const DisplayPiece* piece = frame_.board.pieceAt(cell);
    return piece != nullptr && piece->state == DisplayPieceState::Jumping;
}

void NetworkGameSession::requestMove(const CellCoord& src, const CellCoord& dst) {
    std::lock_guard<std::mutex> lock(mutex_);
    Network::MessageFramer outbound;
    outbound.sendMessage(socket_, Protocol::WireCodec::encode(Protocol::MoveMessage{ src, dst }));
}

void NetworkGameSession::requestJump(const CellCoord& cell) {
    std::lock_guard<std::mutex> lock(mutex_);
    Network::MessageFramer outbound;
    outbound.sendMessage(socket_, Protocol::WireCodec::encode(Protocol::JumpMessage{ cell }));
}

bool NetworkGameSession::waitForLegalMoves(const CellCoord& src, std::vector<CellCoord>& out) const {
    pendingLegal_.reset();
    Network::MessageFramer outbound;
    outbound.sendMessage(socket_, Protocol::WireCodec::encode(Protocol::LegalMovesRequest{ src }));

    const auto deadline =
        std::chrono::steady_clock::now()
        + std::chrono::milliseconds(Kfc::Server::kLegalMovesTimeoutMs);

    while (std::chrono::steady_clock::now() < deadline) {
        pollIncoming();
        if (pendingLegal_.has_value() && pendingLegal_->src == src) {
            out = pendingLegal_->destinations;
            pendingLegal_.reset();
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Kfc::Server::kSocketPollMs));
    }
    return false;
}

std::vector<CellCoord> NetworkGameSession::legalDestinations(const CellCoord& src) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<CellCoord> destinations;
    waitForLegalMoves(src, destinations);
    return destinations;
}

std::vector<DisplayMotion> NetworkGameSession::activeMotions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.motions;
}

std::vector<DisplayJump> NetworkGameSession::activeJumps() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_.jumps;
}
