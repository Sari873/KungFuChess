#include "ScriptRunner.h"
#include "../Model/Board.h"
#include "../Model/GameState.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include "../I_O/BoardPrinter.h"

#include <istream>
#include <ostream>
#include <sstream>
#include <memory>
#include <set>

bool ScriptRunner::isValidToken(const std::string& token) {
    static const std::set<std::string> valid = {
        ".",
        "wK", "wQ", "wR", "wB", "wN", "wP",
        "bK", "bQ", "bR", "bB", "bN", "bP"
    };
    return valid.count(token) > 0;
}

Board ScriptRunner::buildBoard(const std::vector<std::vector<std::string>>& grid) {
    int rows = static_cast<int>(grid.size());
    int cols = grid.empty() ? 0 : static_cast<int>(grid[0].size());

    Board board(rows, cols);
    int nextId = 1;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const std::string& token = grid[r][c];
            if (token == ".") continue;

            PieceColor color = Piece::colorFromChar(token[0]);
            PieceKind  kind = Piece::kindFromChar(token[1]);
            board.addPiece(std::make_unique<Piece>(nextId++, color, kind, Position(r, c)));
        }
    }
    return board;
}

bool ScriptRunner::parseBoardLine(const std::string& line, std::ostream& out) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> row;

    while (ss >> token) {
        if (!isValidToken(token)) {
            out << "ERROR UNKNOWN_TOKEN\n";
            return false;
        }
        row.push_back(token);
    }
    if (row.empty()) return true;

    if (rowWidth_ == 0) {
        rowWidth_ = row.size();
    }
    else if (row.size() != rowWidth_) {
        out << "ERROR ROW_WIDTH_MISMATCH\n";
        return false;
    }

    grid_.push_back(row);
    return true;
}

void ScriptRunner::processCommand(const std::string& line, GameEngine& engine,
    Controller& controller, std::ostream& out) {
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if (cmd == "click") {
        int x, y;
        if (ss >> x >> y) {
            controller.handleClick(x, y);
        }
    }
    else if (cmd == "wait") {
        int ms;
        if (ss >> ms) {
            engine.advanceTime(ms);
        }
    }
    else if (cmd == "print") {
        std::string sub;
        ss >> sub;
        if (sub == "board") {
            BoardPrinter::print(engine.getBoard(), out);
        }
    }
}

void ScriptRunner::run(std::istream& in, std::ostream& out) {
    std::vector<std::string> commandLines;
    bool parsingBoard = false;
    bool parsingCommands = false;
    std::string line;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.find("Board:") != std::string::npos) {
            parsingBoard = true;
            parsingCommands = false;
            continue;
        }
        if (line.find("Commands:") != std::string::npos) {
            parsingBoard = false;
            parsingCommands = true;
            continue;
        }

        if (line.find_first_not_of(" \t") == std::string::npos) continue;

        if (parsingBoard) {
            if (!parseBoardLine(line, out)) return; // parse error already printed
        }
        else if (parsingCommands) {
            commandLines.push_back(line);
        }
    }

    GameEngine engine{ GameState(buildBoard(grid_)) };
    Controller controller(engine);

    for (const std::string& cmd : commandLines) {
        processCommand(cmd, engine, controller, out);
    }
}
