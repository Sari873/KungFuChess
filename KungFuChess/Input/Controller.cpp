#include "Controller.h"
#include "BoardMapper.h"

void Controller::IdleState::onClick(Controller& controller, bool inBoard, const CellCoord& cell) {
    if (!inBoard) {
        return;
    }

    const IGameSession& session = controller.session();
    if (session.isEmpty(cell)) {
        return;
    }
    if (session.isMoving(cell) || session.isJumping(cell)) {
        return;
    }

    controller.select(cell);
}

void Controller::SelectedState::onClick(Controller& controller, bool inBoard, const CellCoord& cell) {
    if (!inBoard) {
        controller.clearSelection();
        return;
    }

    IGameSession& session = controller.session();

    // Re-clicking the selected piece triggers a jump.
    if (cell == controller.getSelected()) {
        session.requestJump(cell);
        controller.clearSelection();
        return;
    }

    session.requestMove(controller.getSelected(), cell);
    controller.clearSelection();
}

Controller::Controller(IGameSession& session)
    : session_(session), state_(std::make_unique<IdleState>()) {
}

void Controller::handleClick(int x, int y) {
    if (session_.isGameOver()) {
        return;
    }

    const DisplayBoard board = session_.board();
    CellCoord cell;
    const bool inBoard = BoardMapper::pixelsToCell(x, y, board.rows(), board.cols(), cell);
    state_->onClick(*this, inBoard, cell);
}

void Controller::resetSelection() {
    clearSelection();
}

bool Controller::hasSelection() const {
    return hasSelection_;
}

CellCoord Controller::getSelected() const {
    return selected_;
}

IGameSession& Controller::session() {
    return session_;
}

const IGameSession& Controller::session() const {
    return session_;
}

void Controller::clearSelection() {
    hasSelection_ = false;
    setState(std::make_unique<IdleState>());
}

void Controller::select(const CellCoord& cell) {
    selected_ = cell;
    hasSelection_ = true;
    setState(std::make_unique<SelectedState>());
}

void Controller::setState(std::unique_ptr<ISelectionState> state) {
    state_ = std::move(state);
}
