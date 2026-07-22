#include "GameFrameCodec.h"
#include "../GameConstants.h"
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace Protocol {
namespace {

const char* colorToString(DisplayColor color) {
    return color == DisplayColor::White ? "White" : "Black";
}

std::optional<DisplayColor> colorFromString(const std::string& s) {
    if (s == "White") return DisplayColor::White;
    if (s == "Black") return DisplayColor::Black;
    return std::nullopt;
}

const char* kindToString(DisplayKind kind) {
    switch (kind) {
    case DisplayKind::King: return "King";
    case DisplayKind::Queen: return "Queen";
    case DisplayKind::Rook: return "Rook";
    case DisplayKind::Bishop: return "Bishop";
    case DisplayKind::Knight: return "Knight";
    case DisplayKind::Pawn: return "Pawn";
    }
    return "Pawn";
}

std::optional<DisplayKind> kindFromString(const std::string& s) {
    if (s == "King") return DisplayKind::King;
    if (s == "Queen") return DisplayKind::Queen;
    if (s == "Rook") return DisplayKind::Rook;
    if (s == "Bishop") return DisplayKind::Bishop;
    if (s == "Knight") return DisplayKind::Knight;
    if (s == "Pawn") return DisplayKind::Pawn;
    return std::nullopt;
}

const char* stateToString(DisplayPieceState state) {
    switch (state) {
    case DisplayPieceState::Idle: return "Idle";
    case DisplayPieceState::Moving: return "Moving";
    case DisplayPieceState::Jumping: return "Jumping";
    case DisplayPieceState::Captured: return "Captured";
    }
    return "Idle";
}

std::optional<DisplayPieceState> stateFromString(const std::string& s) {
    if (s == "Idle") return DisplayPieceState::Idle;
    if (s == "Moving") return DisplayPieceState::Moving;
    if (s == "Jumping") return DisplayPieceState::Jumping;
    if (s == "Captured") return DisplayPieceState::Captured;
    return std::nullopt;
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

void appendPiece(std::ostringstream& out, const DisplayPiece& piece) {
    out << "{\"id\":" << piece.id
        << ",\"color\":";
    appendEscaped(out, colorToString(piece.color));
    out << ",\"kind\":";
    appendEscaped(out, kindToString(piece.kind));
    out << ",\"cell\":";
    appendCell(out, piece.cell);
    out << ",\"state\":";
    appendEscaped(out, stateToString(piece.state));
    out << '}';
}

void appendMotion(std::ostringstream& out, const DisplayMotion& motion) {
    out << "{\"pieceId\":" << motion.pieceId
        << ",\"src\":";
    appendCell(out, motion.src);
    out << ",\"dst\":";
    appendCell(out, motion.dst);
    out << ",\"startMs\":" << motion.startMs
        << ",\"durationMs\":" << motion.durationMs
        << '}';
}

void appendJump(std::ostringstream& out, const DisplayJump& jump) {
    out << "{\"pieceId\":" << jump.pieceId
        << ",\"src\":";
    appendCell(out, jump.src);
    out << ",\"dst\":";
    appendCell(out, jump.dst);
    out << ",\"startMs\":" << jump.startMs
        << ",\"durationMs\":" << jump.durationMs
        << '}';
}

DisplayBoard rebuildBoard(int rows, int cols, std::vector<DisplayPiece> pieces) {
    std::vector<std::optional<DisplayPiece>> cells(
        static_cast<std::size_t>(rows * cols));
    for (const DisplayPiece& piece : pieces) {
        if (piece.state == DisplayPieceState::Moving
            || piece.state == DisplayPieceState::Captured) {
            continue;
        }
        if (piece.cell.row < Kfc::Grid::kNeutral || piece.cell.row >= rows
            || piece.cell.col < Kfc::Grid::kNeutral || piece.cell.col >= cols) {
            continue;
        }
        const std::size_t index =
            static_cast<std::size_t>(piece.cell.row * cols + piece.cell.col);
        cells[index] = piece;
    }
    return DisplayBoard(rows, cols, std::move(cells), std::move(pieces));
}

class JsonCursor {
public:
    explicit JsonCursor(const std::string& text) : text_(text) {}

    void skipWs() {
        while (pos_ < text_.size()
               && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    bool consume(char expected) {
        skipWs();
        if (pos_ >= text_.size() || text_[pos_] != expected) {
            return false;
        }
        ++pos_;
        return true;
    }

    bool peek(char expected) {
        skipWs();
        return pos_ < text_.size() && text_[pos_] == expected;
    }

    bool expect(char expected) {
        if (!consume(expected)) {
            throw std::runtime_error(std::string("expected '") + expected + "'");
        }
        return true;
    }

    std::string parseString() {
        expect('"');
        std::string out;
        while (pos_ < text_.size()) {
            const char c = text_[pos_++];
            if (c == '"') {
                return out;
            }
            if (c == '\\' && pos_ < text_.size()) {
                out.push_back(text_[pos_++]);
            } else {
                out.push_back(c);
            }
        }
        throw std::runtime_error("unterminated string");
    }

    long long parseLong() {
        skipWs();
        std::size_t start = pos_;
        if (pos_ < text_.size() && (text_[pos_] == '-' || text_[pos_] == '+')) {
            ++pos_;
        }
        while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
        if (start == pos_ || (pos_ == start + 1 && !std::isdigit(static_cast<unsigned char>(text_[start])))) {
            throw std::runtime_error("expected number");
        }
        return std::stoll(text_.substr(start, pos_ - start));
    }

    int parseInt() {
        return static_cast<int>(parseLong());
    }

    bool parseBool() {
        skipWs();
        if (text_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            return true;
        }
        if (text_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            return false;
        }
        throw std::runtime_error("expected bool");
    }

    void expectKey(const char* key) {
        const std::string found = parseString();
        if (found != key) {
            throw std::runtime_error(std::string("expected key ") + key + " got " + found);
        }
        expect(':');
    }

    std::optional<DisplayColor> parseOptionalColor() {
        skipWs();
        if (text_.compare(pos_, 4, "null") == 0) {
            pos_ += 4;
            return std::nullopt;
        }
        const auto color = colorFromString(parseString());
        if (!color) {
            throw std::runtime_error("invalid color");
        }
        return color;
    }

    CellCoord parseCell() {
        expect('{');
        expectKey("row");
        const int row = parseInt();
        expect(',');
        expectKey("col");
        const int col = parseInt();
        expect('}');
        return CellCoord(row, col);
    }

    DisplayPiece parsePiece() {
        DisplayPiece piece;
        expect('{');
        expectKey("id");
        piece.id = parseInt();
        expect(',');
        expectKey("color");
        const auto color = colorFromString(parseString());
        if (!color) throw std::runtime_error("bad piece color");
        piece.color = *color;
        expect(',');
        expectKey("kind");
        const auto kind = kindFromString(parseString());
        if (!kind) throw std::runtime_error("bad piece kind");
        piece.kind = *kind;
        expect(',');
        expectKey("cell");
        piece.cell = parseCell();
        expect(',');
        expectKey("state");
        const auto state = stateFromString(parseString());
        if (!state) throw std::runtime_error("bad piece state");
        piece.state = *state;
        expect('}');
        return piece;
    }

    DisplayMotion parseMotion() {
        DisplayMotion motion;
        expect('{');
        expectKey("pieceId");
        motion.pieceId = parseInt();
        expect(',');
        expectKey("src");
        motion.src = parseCell();
        expect(',');
        expectKey("dst");
        motion.dst = parseCell();
        expect(',');
        expectKey("startMs");
        motion.startMs = parseLong();
        expect(',');
        expectKey("durationMs");
        motion.durationMs = parseLong();
        expect('}');
        return motion;
    }

    DisplayJump parseJump() {
        DisplayJump jump;
        expect('{');
        expectKey("pieceId");
        jump.pieceId = parseInt();
        expect(',');
        expectKey("src");
        jump.src = parseCell();
        expect(',');
        expectKey("dst");
        jump.dst = parseCell();
        expect(',');
        expectKey("startMs");
        jump.startMs = parseLong();
        expect(',');
        expectKey("durationMs");
        jump.durationMs = parseLong();
        expect('}');
        return jump;
    }

    GameFrame parseFrame() {
        GameFrame frame;
        expect('{');
        expectKey("clockMs");
        frame.clockMs = parseLong();
        expect(',');
        expectKey("gameOver");
        frame.gameOver = parseBool();
        expect(',');
        expectKey("winner");
        frame.winner = parseOptionalColor();
        expect(',');
        expectKey("board");
        expect('{');
        expectKey("rows");
        const int rows = parseInt();
        expect(',');
        expectKey("cols");
        const int cols = parseInt();
        expect(',');
        expectKey("pieces");
        expect('[');
        std::vector<DisplayPiece> pieces;
        if (!peek(']')) {
            while (true) {
                pieces.push_back(parsePiece());
                if (peek(']')) {
                    break;
                }
                expect(',');
            }
        }
        expect(']');
        expect('}');
        frame.board = rebuildBoard(rows, cols, std::move(pieces));
        expect(',');
        expectKey("motions");
        expect('[');
        if (!peek(']')) {
            while (true) {
                frame.motions.push_back(parseMotion());
                if (peek(']')) {
                    break;
                }
                expect(',');
            }
        }
        expect(']');
        expect(',');
        expectKey("jumps");
        expect('[');
        if (!peek(']')) {
            while (true) {
                frame.jumps.push_back(parseJump());
                if (peek(']')) {
                    break;
                }
                expect(',');
            }
        }
        expect(']');
        expect('}');
        return frame;
    }

private:
    const std::string& text_;
    std::size_t pos_ = 0;
};

bool piecesEqual(const DisplayPiece& a, const DisplayPiece& b) {
    return a.id == b.id && a.color == b.color && a.kind == b.kind
        && a.cell == b.cell && a.state == b.state;
}

bool motionsEqual(const DisplayMotion& a, const DisplayMotion& b) {
    return a.pieceId == b.pieceId && a.src == b.src && a.dst == b.dst
        && a.startMs == b.startMs && a.durationMs == b.durationMs;
}

bool jumpsEqual(const DisplayJump& a, const DisplayJump& b) {
    return a.pieceId == b.pieceId && a.src == b.src && a.dst == b.dst
        && a.startMs == b.startMs && a.durationMs == b.durationMs;
}

} 

std::string GameFrameCodec::encode(const GameFrame& frame) {
    std::ostringstream out;
    out << "{\"clockMs\":" << frame.clockMs
        << ",\"gameOver\":" << (frame.gameOver ? "true" : "false")
        << ",\"winner\":";
    if (frame.winner.has_value()) {
        appendEscaped(out, colorToString(*frame.winner));
    } else {
        out << "null";
    }

    out << ",\"board\":{\"rows\":" << frame.board.rows()
        << ",\"cols\":" << frame.board.cols()
        << ",\"pieces\":[";
    const auto& pieces = frame.board.piecesById();
    for (std::size_t i = 0; i < pieces.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        appendPiece(out, pieces[i]);
    }
    out << "]},\"motions\":[";
    for (std::size_t i = 0; i < frame.motions.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        appendMotion(out, frame.motions[i]);
    }
    out << "],\"jumps\":[";
    for (std::size_t i = 0; i < frame.jumps.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        appendJump(out, frame.jumps[i]);
    }
    out << "]}";
    return out.str();
}

std::optional<GameFrame> GameFrameCodec::decode(const std::string& json) {
    try {
        JsonCursor cursor(json);
        return cursor.parseFrame();
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

bool gameFramesEqual(const GameFrame& a, const GameFrame& b) {
    if (a.clockMs != b.clockMs || a.gameOver != b.gameOver || a.winner != b.winner) {
        return false;
    }
    if (a.board.rows() != b.board.rows() || a.board.cols() != b.board.cols()) {
        return false;
    }
    if (a.board.piecesById().size() != b.board.piecesById().size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.board.piecesById().size(); ++i) {
        if (!piecesEqual(a.board.piecesById()[i], b.board.piecesById()[i])) {
            return false;
        }
    }
    if (a.motions.size() != b.motions.size() || a.jumps.size() != b.jumps.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.motions.size(); ++i) {
        if (!motionsEqual(a.motions[i], b.motions[i])) {
            return false;
        }
    }
    for (std::size_t i = 0; i < a.jumps.size(); ++i) {
        if (!jumpsEqual(a.jumps[i], b.jumps[i])) {
            return false;
        }
    }
    return true;
}

} 
