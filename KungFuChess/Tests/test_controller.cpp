#define TEST_SUITE_NAME "Controller"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../Adapter/GameEngineSession.h"
#include "../Engine/GameEngine.h"
#include "../GameConstants.h"
#include "../Input/Controller.h"
#include "../Model/GameState.h"

static GameEngine makeEngine() {
    Board board(Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent);
    put(board, PieceColor::White, PieceKind::Rook, Kfc::Grid::kNeutral, Kfc::Grid::kNeutral);
    return GameEngine{ GameState(std::move(board)) };
}

TEST_CASE("the first click selects a piece") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    check(controller.hasSelection(), "something is selected");
    check(controller.getSelected() == CellCoord(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral),
          "and it is the right cell");
}

TEST_CASE("clicking an empty cell selects nothing") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kForward), px(Kfc::Grid::kForward));
    check(!controller.hasSelection(), "an empty cell cannot be picked up");
}

TEST_CASE("clicking off the board selects nothing and does not crash") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(Kfc::Test::kOffScreenClickPx, Kfc::Test::kOffScreenClickPx);
    check(!controller.hasSelection(), "nothing selected");
}

TEST_CASE("the second click issues the move, which lands later") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    controller.handleClick(px(Kfc::Grid::kForward), px(Kfc::Grid::kNeutral));

    check(!controller.hasSelection(), "the selection is cleared");
    check(engine.getBoard().isEmpty(Position(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral)),
          "the origin is vacated at launch");
    check(engine.hasActiveMotions(), "the piece is in flight");

    engine.advanceTime(Kfc::Test::kSingleCellTravelMs);
    check(!engine.getBoard().isEmpty(Position(Kfc::Grid::kNeutral, Kfc::Grid::kForward)),
          "the piece arrives after enough time");
}

TEST_CASE("an illegal second click still clears the selection") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    controller.handleClick(px(Kfc::Grid::kForward), px(Kfc::Grid::kForward));

    check(!controller.hasSelection(), "the selection is cleared even though the move failed");

    engine.advanceTime(Kfc::Test::kAbsurdWaitMs);
    check(!engine.getBoard().isEmpty(Position(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral)),
          "and nothing was launched");
}

TEST_CASE("a second click off the board cancels without a command") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    controller.handleClick(Kfc::Test::kOffScreenClickPx, Kfc::Test::kOffScreenClickPx);

    check(!controller.hasSelection(), "the selection is cancelled");

    engine.advanceTime(Kfc::Test::kAbsurdWaitMs);
    check(!engine.getBoard().isEmpty(Position(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral)),
          "no command was sent");
}

TEST_CASE("a piece in flight cannot be picked up again") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    controller.handleClick(px(Kfc::Test::kTwoCellMoveIndex), px(Kfc::Grid::kNeutral));
    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));

    check(!controller.hasSelection(), "the flying piece cannot be selected");
}

TEST_CASE("resetSelection clears an active selection") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    check(controller.hasSelection(), "selected");
    controller.resetSelection();
    check(!controller.hasSelection(), "cleared");
}

TEST_CASE("clicking the same piece twice starts a jump") {
    GameEngine engine = makeEngine();
    GameEngineSession session(engine);
    Controller controller(session);

    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));
    controller.handleClick(px(Kfc::Grid::kNeutral), px(Kfc::Grid::kNeutral));

    check(!controller.hasSelection(), "the selection is cleared");
    check(engine.isJumping(Position(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral)), "re-click jumps in place");
    check(!engine.getBoard().isEmpty(Position(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral)),
          "the piece stayed on its cell");

    engine.advanceTime(Kfc::Test::kSingleCellTravelMs);
    check(!engine.isJumping(Position(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral)),
          "jump finishes after 1000ms");
}
