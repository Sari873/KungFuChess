#include "WireMessage.h"
#include "GameFrameCodec.h"
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace Protocol {
namespace {

void skipWs(const std::string& text, std::size_t& pos) {
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
}

bool consume(const std::string& text, std::size_t& pos, char expected) {
    skipWs(text, pos);
    if (pos >= text.size() || text[pos] != expected) {
        return false;
    }
    ++pos;
    return true;
}

void expect(const std::string& text, std::size_t& pos, char expected) {
    if (!consume(text, pos, expected)) {
        throw std::runtime_error(std::string("wire expected '") + expected + "'");
    }
}

std::string parseString(const std::string& text, std::size_t& pos) {
    expect(text, pos, '"');
    std::string out;
    while (pos < text.size()) {
        const char c = text[pos++];
        if (c == '"') {
            return out;
        }
        if (c == '\\' && pos < text.size()) {
            out.push_back(text[pos++]);
        } else {
            out.push_back(c);
        }
    }
    throw std::runtime_error("unterminated wire string");
}

long long parseLong(const std::string& text, std::size_t& pos) {
    skipWs(text, pos);
    const std::size_t start = pos;
    if (pos < text.size() && (text[pos] == '-' || text[pos] == '+')) {
        ++pos;
    }
    while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
    if (start == pos) {
        throw std::runtime_error("expected number");
    }
    return std::stoll(text.substr(start, pos - start));
}

int parseInt(const std::string& text, std::size_t& pos) {
    return static_cast<int>(parseLong(text, pos));
}

void expectKey(const std::string& text, std::size_t& pos, const char* key) {
    const std::string found = parseString(text, pos);
    if (found != key) {
        throw std::runtime_error(std::string("expected key ") + key);
    }
    expect(text, pos, ':');
}

CellCoord parseCell(const std::string& text, std::size_t& pos) {
    expect(text, pos, '{');
    expectKey(text, pos, "row");
    const int row = parseInt(text, pos);
    expect(text, pos, ',');
    expectKey(text, pos, "col");
    const int col = parseInt(text, pos);
    expect(text, pos, '}');
    return CellCoord(row, col);
}

void appendEscaped(std::ostringstream& out, const std::string& s) {
    out << '"';
    for (char c : s) {
        if (c == '"' || c == '\\') {
            out << '\\' << c;
        } else {
            out << c;
        }
    }
    out << '"';
}

void appendCell(std::ostringstream& out, const CellCoord& cell) {
    out << "{\"row\":" << cell.row << ",\"col\":" << cell.col << '}';
}

std::string findType(const std::string& json) {
    const auto typePos = json.find("\"type\"");
    if (typePos == std::string::npos) {
        throw std::runtime_error("missing type");
    }
    std::size_t pos = typePos + 6;
    expect(json, pos, ':');
    return parseString(json, pos);
}

bool findIntField(const std::string& json, const char* key, int& out) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto keyPos = json.find(needle);
    if (keyPos == std::string::npos) {
        return false;
    }
    std::size_t pos = keyPos + needle.size();
    expect(json, pos, ':');
    out = parseInt(json, pos);
    return true;
}

bool findStringField(const std::string& json, const char* key, std::string& out) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto keyPos = json.find(needle);
    if (keyPos == std::string::npos) {
        return false;
    }
    std::size_t pos = keyPos + needle.size();
    expect(json, pos, ':');
    out = parseString(json, pos);
    return true;
}

std::string extractObjectAfterKey(const std::string& json, const char* key) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto keyPos = json.find(needle);
    if (keyPos == std::string::npos) {
        throw std::runtime_error(std::string("missing ") + key);
    }
    std::size_t pos = keyPos + needle.size();
    expect(json, pos, ':');
    skipWs(json, pos);
    if (pos >= json.size() || json[pos] != '{') {
        throw std::runtime_error("expected object payload");
    }
    const std::size_t start = pos;
    int depth = 0;
    bool inString = false;
    bool escape = false;
    for (; pos < json.size(); ++pos) {
        const char c = json[pos];
        if (inString) {
            if (escape) {
                escape = false;
            } else if (c == '\\') {
                escape = true;
            } else if (c == '"') {
                inString = false;
            }
            continue;
        }
        if (c == '"') {
            inString = true;
        } else if (c == '{') {
            ++depth;
        } else if (c == '}') {
            --depth;
            if (depth == 0) {
                return json.substr(start, pos - start + 1);
            }
        }
    }
    throw std::runtime_error("unterminated object");
}

} 

std::string WireCodec::encode(const WireMessage& message) {
    std::ostringstream out;
    if (std::holds_alternative<WelcomeMessage>(message)) {
        out << "{\"type\":\"Welcome\",\"playerId\":" << std::get<WelcomeMessage>(message).playerId << '}';
        return out.str();
    }
    if (std::holds_alternative<FrameMessage>(message)) {
        out << "{\"type\":\"Frame\",\"payload\":"
            << GameFrameCodec::encode(std::get<FrameMessage>(message).frame) << '}';
        return out.str();
    }
    if (std::holds_alternative<MoveMessage>(message)) {
        const auto& m = std::get<MoveMessage>(message);
        out << "{\"type\":\"Move\",\"src\":";
        appendCell(out, m.src);
        out << ",\"dst\":";
        appendCell(out, m.dst);
        out << '}';
        return out.str();
    }
    if (std::holds_alternative<JumpMessage>(message)) {
        out << "{\"type\":\"Jump\",\"cell\":";
        appendCell(out, std::get<JumpMessage>(message).cell);
        out << '}';
        return out.str();
    }
    if (std::holds_alternative<LegalMovesRequest>(message)) {
        out << "{\"type\":\"LegalMovesRequest\",\"src\":";
        appendCell(out, std::get<LegalMovesRequest>(message).src);
        out << '}';
        return out.str();
    }
    if (std::holds_alternative<LegalMovesReply>(message)) {
        const auto& m = std::get<LegalMovesReply>(message);
        out << "{\"type\":\"LegalMovesReply\",\"src\":";
        appendCell(out, m.src);
        out << ",\"destinations\":[";
        for (std::size_t i = 0; i < m.destinations.size(); ++i) {
            if (i > 0) out << ',';
            appendCell(out, m.destinations[i]);
        }
        out << "]}";
        return out.str();
    }
    if (std::holds_alternative<RejectMessage>(message)) {
        out << "{\"type\":\"Reject\",\"reason\":";
        appendEscaped(out, std::get<RejectMessage>(message).reason);
        out << '}';
        return out.str();
    }
    if (std::holds_alternative<HelloMessage>(message)) {
        const auto& m = std::get<HelloMessage>(message);
        out << "{\"type\":\"Hello\",\"name\":";
        appendEscaped(out, m.name);
        out << ",\"rating\":" << m.rating << '}';
        return out.str();
    }
    if (std::holds_alternative<FindMatchMessage>(message)) {
        return "{\"type\":\"FindMatch\"}";
    }
    if (std::holds_alternative<CancelFindMessage>(message)) {
        return "{\"type\":\"CancelFind\"}";
    }
    if (std::holds_alternative<CreateRoomMessage>(message)) {
        return "{\"type\":\"CreateRoom\"}";
    }
    if (std::holds_alternative<JoinRoomMessage>(message)) {
        out << "{\"type\":\"JoinRoom\",\"code\":";
        appendEscaped(out, std::get<JoinRoomMessage>(message).code);
        out << '}';
        return out.str();
    }
    if (std::holds_alternative<RoomCreatedMessage>(message)) {
        out << "{\"type\":\"RoomCreated\",\"code\":";
        appendEscaped(out, std::get<RoomCreatedMessage>(message).code);
        out << '}';
        return out.str();
    }
    if (std::holds_alternative<QueueStatusMessage>(message)) {
        out << "{\"type\":\"QueueStatus\",\"position\":"
            << std::get<QueueStatusMessage>(message).position << '}';
        return out.str();
    }
    if (std::holds_alternative<RoomWaitingMessage>(message)) {
        const auto& m = std::get<RoomWaitingMessage>(message);
        out << "{\"type\":\"RoomWaiting\",\"code\":";
        appendEscaped(out, m.code);
        out << ",\"players\":" << m.players << '}';
        return out.str();
    }
    if (std::holds_alternative<MatchReadyMessage>(message)) {
        const auto& m = std::get<MatchReadyMessage>(message);
        out << "{\"type\":\"MatchReady\",\"matchId\":" << m.matchId
            << ",\"yourColor\":";
        appendEscaped(out, colorToWire(m.yourColor));
        out << ",\"opponentName\":";
        appendEscaped(out, m.opponentName);
        out << ",\"opponentRating\":" << m.opponentRating << '}';
        return out.str();
    }
    out << "{\"type\":\"LobbyError\",\"reason\":";
    appendEscaped(out, std::get<LobbyErrorMessage>(message).reason);
    out << '}';
    return out.str();
}

std::optional<WireMessage> WireCodec::decode(const std::string& json) {
    try {
        const std::string type = findType(json);
        if (type == "Welcome") {
            WelcomeMessage msg;
            findIntField(json, "playerId", msg.playerId);
            return msg;
        }
        if (type == "Frame") {
            const auto frame = GameFrameCodec::decode(extractObjectAfterKey(json, "payload"));
            if (!frame) return std::nullopt;
            return FrameMessage{ *frame };
        }
        if (type == "Move") {
            MoveMessage msg;
            std::size_t pos = json.find("\"src\"");
            if (pos == std::string::npos) return std::nullopt;
            pos += 5;
            expect(json, pos, ':');
            msg.src = parseCell(json, pos);
            pos = json.find("\"dst\"");
            if (pos == std::string::npos) return std::nullopt;
            pos += 5;
            expect(json, pos, ':');
            msg.dst = parseCell(json, pos);
            return msg;
        }
        if (type == "Jump") {
            JumpMessage msg;
            std::size_t pos = json.find("\"cell\"");
            if (pos == std::string::npos) return std::nullopt;
            pos += 6;
            expect(json, pos, ':');
            msg.cell = parseCell(json, pos);
            return msg;
        }
        if (type == "LegalMovesRequest") {
            LegalMovesRequest msg;
            std::size_t pos = json.find("\"src\"");
            if (pos == std::string::npos) return std::nullopt;
            pos += 5;
            expect(json, pos, ':');
            msg.src = parseCell(json, pos);
            return msg;
        }
        if (type == "LegalMovesReply") {
            LegalMovesReply msg;
            std::size_t pos = json.find("\"src\"");
            if (pos == std::string::npos) return std::nullopt;
            pos += 5;
            expect(json, pos, ':');
            msg.src = parseCell(json, pos);
            pos = json.find("\"destinations\"");
            if (pos == std::string::npos) return std::nullopt;
            pos += 14;
            expect(json, pos, ':');
            expect(json, pos, '[');
            skipWs(json, pos);
            if (!(pos < json.size() && json[pos] == ']')) {
                while (true) {
                    msg.destinations.push_back(parseCell(json, pos));
                    skipWs(json, pos);
                    if (pos < json.size() && json[pos] == ']') break;
                    expect(json, pos, ',');
                }
            }
            expect(json, pos, ']');
            return msg;
        }
        if (type == "Reject") {
            RejectMessage msg;
            findStringField(json, "reason", msg.reason);
            return msg;
        }
        if (type == "Hello") {
            HelloMessage msg;
            findStringField(json, "name", msg.name);
            findIntField(json, "rating", msg.rating);
            return msg;
        }
        if (type == "FindMatch") return FindMatchMessage{};
        if (type == "CancelFind") return CancelFindMessage{};
        if (type == "CreateRoom") return CreateRoomMessage{};
        if (type == "JoinRoom") {
            JoinRoomMessage msg;
            findStringField(json, "code", msg.code);
            return msg;
        }
        if (type == "RoomCreated") {
            RoomCreatedMessage msg;
            findStringField(json, "code", msg.code);
            return msg;
        }
        if (type == "QueueStatus") {
            QueueStatusMessage msg;
            findIntField(json, "position", msg.position);
            return msg;
        }
        if (type == "RoomWaiting") {
            RoomWaitingMessage msg;
            findStringField(json, "code", msg.code);
            findIntField(json, "players", msg.players);
            return msg;
        }
        if (type == "MatchReady") {
            MatchReadyMessage msg;
            findIntField(json, "matchId", msg.matchId);
            std::string color;
            findStringField(json, "yourColor", color);
            if (auto c = colorFromWire(color)) {
                msg.yourColor = *c;
            }
            findStringField(json, "opponentName", msg.opponentName);
            findIntField(json, "opponentRating", msg.opponentRating);
            return msg;
        }
        if (type == "LobbyError") {
            LobbyErrorMessage msg;
            findStringField(json, "reason", msg.reason);
            return msg;
        }
        return std::nullopt;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

} 
