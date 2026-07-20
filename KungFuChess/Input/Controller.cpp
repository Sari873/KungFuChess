#include "Controller.h"
#include "BoardMapper.h"

void Controller::IdleState::onClick(Controller& controller, bool inBoard, const Position& cell) {
    if (!inBoard) {
        return;
    }

    const Board& board = controller.engine().getBoard();
    if (board.isEmpty(cell)) {
        return;
    }
    if (controller.engine().isMoving(cell) || controller.engine().isJumping(cell)) {
        return;
    }

    controller.select(cell);
}

void Controller::SelectedState::onClick(Controller& controller, bool inBoard, const Position& cell) {
    if (!inBoard) {
        controller.clearSelection();
        return;
    }

    // Re-clicking the selected piece triggers a jump.
    if (cell == controller.getSelected()) {
        controller.engine().requestJump(cell);
        controller.clearSelection();
        return;
    }

    controller.engine().requestMove(controller.getSelected(), cell);
    controller.clearSelection();
}

Controller::Controller(GameEngine& engine)
    : engine_(engine), state_(std::make_unique<IdleState>()) {
}

void Controller::handleClick(int x, int y) {
    if (engine_.isGameOver()) {
        return;
    }

    Position cell;
    const bool inBoard = BoardMapper::pixelsToCell(x, y, engine_.getBoard(), cell);
    state_->onClick(*this, inBoard, cell);
}

void Controller::resetSelection() {
    clearSelection();
}

bool Controller::hasSelection() const {
    return hasSelection_;
}

Position Controller::getSelected() const {
    return selected_;
}

GameEngine& Controller::engine() {
    return engine_;
}

const GameEngine& Controller::engine() const {
    return engine_;
}

void Controller::clearSelection() {
    hasSelection_ = false;
    setState(std::make_unique<IdleState>());
}

void Controller::select(const Position& cell) {
    selected_ = cell;
    hasSelection_ = true;
    setState(std::make_unique<SelectedState>());
}

void Controller::setState(std::unique_ptr<ISelectionState> state) {
    state_ = std::move(state);
}
