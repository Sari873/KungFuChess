#include "Controller.h"
#include "BoardMapper.h"
#include "RuleEngine.h"

void Controller::resetSelection() {
    hasSelection_ = false;
}

void Controller::handleClick(int x, int y, Board& board) {
    Position pos;
    bool inBoard = BoardMapper::pixelsToCell(x, y, board, pos);

    if (!hasSelection_) {
        // Ignore clicks outside the board when nothing is selected.
        if (!inBoard) return;

        // Ignore first clicks on empty cells.
        if (board.isEmpty(pos)) return;

        selected_ = pos;
        hasSelection_ = true;
        return;
    }

    // A piece is already selected.
    if (!inBoard) {
        // Outside-board click cancels the selection; no command is sent.
        hasSelection_ = false;
        return;
    }

    MoveValidation result = RuleEngine::validateMove(board, selected_, pos);
    if (result.is_valid) {
        board.movePiece(selected_, pos);
    }

    // Selection clears after every second in-board click, legal or not.
    hasSelection_ = false;
}
