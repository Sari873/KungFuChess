#define TEST_SUITE_NAME "BoardPrinter"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../Engine/GameEngine.h"
#include "../I_O/BoardPrinter.h"
#include "../Model/GameState.h"
#include <sstream>

static std::string render(const Board& board) {
    std::ostringstream out;
    BoardPrinter::print(board, out);
    return out.str();
}

TEST_CASE("prints pieces in board notation with dots for empty cells") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 2);
    put(board, PieceColor::White, PieceKind::Knight, 2, 1);

    checkEq(render(board), "wR . bK\n. . .\n. wN .\n", "the whole board renders exactly");
}

TEST_CASE("prints an empty board as dots") {
    Board board(2, 2);
    checkEq(render(board), ". .\n. .\n", "all empty");
}

TEST_CASE("prints a non-square board correctly") {
    Board board(1, 4);
    put(board, PieceColor::Black, PieceKind::Queen, 0, 3);
    checkEq(render(board), ". . . bQ\n", "one row, four cols");
}

TEST_CASE("an in-flight piece is printed at its ORIGIN, not its destination") {
    // This is exactly what the real-time iteration is tested on.
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 2));
    checkEq(render(engine.getBoard()), "wR . .\n. . .\n. . .\n", "the rook is still shown at (0,0)");

    engine.advanceTime(1000);
    checkEq(render(engine.getBoard()), "wR . .\n. . .\n. . .\n", "still at (0,0) half way through");

    engine.advanceTime(1000);
    checkEq(render(engine.getBoard()), ". . wR\n. . .\n. . .\n", "only now does it appear at (0,2)");
}

TEST_CASE("a captured piece disappears from the printed board") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 1);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 1));
    engine.advanceTime(1000);

    checkEq(render(engine.getBoard()), ". wR .\n. . .\n. . .\n", "the black king is gone");
}
