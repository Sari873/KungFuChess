#define TEST_SUITE_NAME "Controller"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../Engine/GameEngine.h"
#include "../Input/Controller.h"
#include "../Model/GameState.h"

// Builds a 3x3 board with a white rook at (0,0).
static GameEngine makeEngine() {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    return GameEngine{GameState(std::move(board))};
}

TEST_CASE("the first click selects a piece") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    check(controller.hasSelection(), "something is selected");
    check(controller.getSelected() == Position(0, 0), "and it is the right cell");
}

TEST_CASE("clicking an empty cell selects nothing") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(1), px(1));
    check(!controller.hasSelection(), "an empty cell cannot be picked up");
}

TEST_CASE("clicking off the board selects nothing and does not crash") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(9999, 9999);
    check(!controller.hasSelection(), "nothing selected");
}

TEST_CASE("the second click issues the move, which lands later") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    controller.handleClick(px(1), px(0));

    check(!controller.hasSelection(), "the selection is cleared");
    check(engine.getBoard().isEmpty(Position(0, 0)), "the origin is vacated at launch");
    check(engine.hasActiveMotions(), "the piece is in flight");

    engine.advanceTime(1000);
    check(!engine.getBoard().isEmpty(Position(0, 1)), "the piece arrives after enough time");
}

TEST_CASE("an illegal second click still clears the selection") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    controller.handleClick(px(1), px(1)); // diagonal - illegal for a rook

    check(!controller.hasSelection(), "the selection is cleared even though the move failed");

    engine.advanceTime(99999);
    check(!engine.getBoard().isEmpty(Position(0, 0)), "and nothing was launched");
}

TEST_CASE("a second click off the board cancels without a command") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    controller.handleClick(9999, 9999);

    check(!controller.hasSelection(), "the selection is cancelled");

    engine.advanceTime(99999);
    check(!engine.getBoard().isEmpty(Position(0, 0)), "no command was sent");
}

TEST_CASE("a piece in flight cannot be picked up again") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    controller.handleClick(px(2), px(0)); // launch a two-cell move
    controller.handleClick(px(0), px(0)); // try to grab it mid-flight

    check(!controller.hasSelection(), "the flying piece cannot be selected");
}

TEST_CASE("resetSelection clears an active selection") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    check(controller.hasSelection(), "selected");
    controller.resetSelection();
    check(!controller.hasSelection(), "cleared");
}

TEST_CASE("clicking the same piece twice starts a jump") {
    GameEngine engine = makeEngine();
    Controller controller(engine);

    controller.handleClick(px(0), px(0));
    controller.handleClick(px(0), px(0));

    check(!controller.hasSelection(), "the selection is cleared");
    check(engine.isJumping(Position(0, 0)), "re-click jumps in place");
    check(!engine.getBoard().isEmpty(Position(0, 0)), "the piece stayed on its cell");

    engine.advanceTime(1000);
    check(!engine.isJumping(Position(0, 0)), "jump finishes after 1000ms");
}
