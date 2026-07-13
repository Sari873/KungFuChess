#pragma once
#include "Board.h"

class GameState {
public:
    GameState() : gameOver_(false) {}
    explicit GameState(Board board) : board_(std::move(board)), gameOver_(false) {}

    Board& getBoard() { return board_; }
    const Board& getBoard() const { return board_; }

    bool isGameOver() const { return gameOver_; }
    void setGameOver(bool value) { gameOver_ = value; }

private:
    Board board_;
    bool gameOver_;
};