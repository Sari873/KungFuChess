#pragma once
#include <string>

// Position represents a board cell (row, col) - not pixels.
// It is a pure value object: it does not know board size, rendering,
// movement rules, or input coordinates. Board-bounds checking belongs to
// Board, not Position.
class Position {
public:
    Position();
    Position(int row, int col);

    int getRow() const;
    int getCol() const;

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

    std::string toString() const;

private:
    int row_;
    int col_;
};
