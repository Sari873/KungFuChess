#include "Controller.h"
#include "BoardMapper.h"

Controller::Controller(GameEngine& engine) : engine_(engine) {}

void Controller::resetSelection() {
    hasSelection_ = false;
}

bool Controller::hasSelection() const {
    return hasSelection_;
}

Position Controller::getSelected() const {
    return selected_;
}

void Controller::handleClick(int x, int y) {
    const Board& board = engine_.getBoard();

    Position pos;
    bool inBoard = BoardMapper::pixelsToCell(x, y, board, pos);

    if (!hasSelection_) {
        if (!inBoard) return;
        if (board.isEmpty(pos)) return;

        if (engine_.isMoving(pos)) return;

        selected_ = pos;
        hasSelection_ = true;
        return;
    }

    if (!inBoard) {
        hasSelection_ = false;
        return;
    }

    engine_.requestMove(selected_, pos);

    hasSelection_ = false;
}
