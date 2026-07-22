#pragma once

#include "../Presentation/DisplayTypes.h"
#include "../Presentation/GameFrame.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Protocol {

struct WelcomeMessage {
    int playerId = 1;
};

struct FrameMessage {
    GameFrame frame;
};

struct MoveMessage {
    CellCoord src;
    CellCoord dst;
};

struct JumpMessage {
    CellCoord cell;
};

struct LegalMovesRequest {
    CellCoord src;
};

struct LegalMovesReply {
    CellCoord src;
    std::vector<CellCoord> destinations;
};

struct RejectMessage {
    std::string reason;
};

struct HelloMessage {
    std::string name;
    int rating = 1000;
};

struct FindMatchMessage {};

struct CancelFindMessage {};

struct CreateRoomMessage {};

struct JoinRoomMessage {
    std::string code;
};

struct RoomCreatedMessage {
    std::string code;
};

struct QueueStatusMessage {
    int position = 1;
};

struct RoomWaitingMessage {
    std::string code;
    int players = 1;
};

struct MatchReadyMessage {
    int matchId = 0;
    DisplayColor yourColor = DisplayColor::White;
    std::string opponentName;
    int opponentRating = 1000;
};

struct LobbyErrorMessage {
    std::string reason;
};

using WireMessage = std::variant<WelcomeMessage,
                                 FrameMessage,
                                 MoveMessage,
                                 JumpMessage,
                                 LegalMovesRequest,
                                 LegalMovesReply,
                                 RejectMessage,
                                 HelloMessage,
                                 FindMatchMessage,
                                 CancelFindMessage,
                                 CreateRoomMessage,
                                 JoinRoomMessage,
                                 RoomCreatedMessage,
                                 QueueStatusMessage,
                                 RoomWaitingMessage,
                                 MatchReadyMessage,
                                 LobbyErrorMessage>;

class WireCodec {
public:
    static std::string encode(const WireMessage& message);
    static std::optional<WireMessage> decode(const std::string& json);
};

inline const char* colorToWire(DisplayColor color) {
    return color == DisplayColor::White ? "White" : "Black";
}

inline std::optional<DisplayColor> colorFromWire(const std::string& s) {
    if (s == "White") return DisplayColor::White;
    if (s == "Black") return DisplayColor::Black;
    return std::nullopt;
}

} 
