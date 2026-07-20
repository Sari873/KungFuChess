#pragma once
#include "Board.h"
#include "Piece.h"
#include <optional>

class GameState {
public:
    GameState() : gameOver_(false) {}
    explicit GameState(Board board) : board_(std::move(board)), gameOver_(false) {}

    Board& getBoard() { return board_; }
    const Board& getBoard() const { return board_; }

    bool isGameOver() const { return gameOver_; }
    std::optional<PieceColor> winner() const { return winner_; }

    void endGame(PieceColor winner) {
        gameOver_ = true;
        winner_ = winner;
    }

    void setGameOver(bool value) {
        gameOver_ = value;
        if (!value) {
            winner_.reset();
        }
    }

private:
    Board board_;
    bool gameOver_;
    std::optional<PieceColor> winner_;
};
