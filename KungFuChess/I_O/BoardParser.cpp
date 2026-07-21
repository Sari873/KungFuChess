#include "BoardParser.h"
#include "../GameConstants.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"

#include <memory>
#include <sstream>
#include <stdexcept>

namespace {

std::vector<std::string> tokenizeRow(const std::string& row) {
    std::vector<std::string> tokens;
    std::istringstream stream(row);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

} 

Board BoardParser::parse(const std::vector<std::string>& rows) {
    if (rows.empty()) {
        throw std::invalid_argument("BoardParser: layout has no rows");
    }

    std::vector<std::vector<std::string>> grid;
    grid.reserve(rows.size());
    for (const std::string& row : rows) {
        grid.push_back(tokenizeRow(row));
    }

    const int numRows = static_cast<int>(grid.size());
    const int numCols = static_cast<int>(grid.front().size());
    if (numCols == Kfc::Grid::kNeutral) {
        throw std::invalid_argument("BoardParser: row 0 is empty");
    }

    for (int r = 0; r < numRows; ++r) {
        if (static_cast<int>(grid[r].size()) != numCols) {
            throw std::invalid_argument(
                "BoardParser: row " + std::to_string(r) + " has " +
                std::to_string(grid[r].size()) + " cells, expected " +
                std::to_string(numCols));
        }
    }

    Board board(numRows, numCols);
    int nextId = Kfc::Limits::kFirstAutoPieceId;

    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < numCols; ++c) {
            const std::string& token = grid[r][c];

            if (token == ".") {
                continue; 
            }
            if (token.size() != static_cast<std::size_t>(Kfc::Limits::kPieceNotationTokenLength)) {
                throw std::invalid_argument(
                    "BoardParser: invalid token \"" + token + "\" at (" +
                    std::to_string(r) + "," + std::to_string(c) + ")");
            }

            PieceColor color;
            PieceKind kind;
            try {
                color = Piece::colorFromChar(token[Kfc::Grid::kNeutral]);
                kind = Piece::kindFromChar(token[Kfc::Grid::kForward]);
            } catch (const std::invalid_argument&) {
                throw std::invalid_argument(
                    "BoardParser: invalid token \"" + token + "\" at (" +
                    std::to_string(r) + "," + std::to_string(c) + ")");
            }

            const bool added = board.addPiece(
                std::make_unique<Piece>(nextId, color, kind, Position(r, c)));
            if (!added) {
                throw std::runtime_error(
                    "BoardParser: could not place piece at (" +
                    std::to_string(r) + "," + std::to_string(c) + ")");
            }
            ++nextId;
        }
    }

    return board;
}
