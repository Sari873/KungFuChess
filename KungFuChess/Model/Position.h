#pragma once
#include <string>

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
