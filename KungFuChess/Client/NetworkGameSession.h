#pragma once

#include "../Network/MessageFramer.h"
#include "../Network/TcpSocket.h"
#include "../Presentation/DisplayTypes.h"
#include "../Presentation/GameFrame.h"
#include "../Presentation/IGameSession.h"
#include "../Protocol/WireMessage.h"
#include <mutex>
#include <optional>
#include <string>
#include <vector>

class NetworkGameSession final : public IGameSession {
public:
    NetworkGameSession(std::string host, unsigned short port);

    /** Take over a lobby connection after MatchReady (same TCP session). */
    NetworkGameSession(Network::WinsockContext winsock,
                      Network::TcpSocket socket,
                      Network::MessageFramer framer,
                      DisplayColor yourColor,
                      Protocol::MatchReadyMessage matchInfo);

    void advanceTime(long long deltaMs) override;
    long long clockMs() const override;

    bool isGameOver() const override;
    std::optional<DisplayColor> winner() const override;

    DisplayBoard board() const override;

    bool isEmpty(const CellCoord& cell) const override;
    bool isMoving(const CellCoord& cell) const override;
    bool isJumping(const CellCoord& cell) const override;

    void requestMove(const CellCoord& src, const CellCoord& dst) override;
    void requestJump(const CellCoord& cell) override;
    std::vector<CellCoord> legalDestinations(const CellCoord& src) const override;

    std::vector<DisplayMotion> activeMotions() const override;
    std::vector<DisplayJump> activeJumps() const override;

    bool hasFrame() const;
    DisplayColor yourColor() const { return yourColor_; }
    const Protocol::MatchReadyMessage& matchInfo() const { return matchInfo_; }

private:
    void pollIncoming() const;
    void applyMessage(const std::string& payload) const;
    bool waitForLegalMoves(const CellCoord& src, std::vector<CellCoord>& out) const;

    Network::WinsockContext winsock_;
    mutable Network::TcpSocket socket_;
    mutable Network::MessageFramer framer_;
    mutable std::mutex mutex_;
    mutable GameFrame frame_;
    mutable bool hasFrame_ = false;
    mutable std::optional<Protocol::LegalMovesReply> pendingLegal_;
    DisplayColor yourColor_ = DisplayColor::White;
    Protocol::MatchReadyMessage matchInfo_;
};
