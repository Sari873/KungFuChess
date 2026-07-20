#define TEST_SUITE_NAME "RealTimeArbiter"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../RealTime/RealTimeArbiter.h"

TEST_CASE("starting a motion vacates the origin immediately") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    const int rookId = board.getPieceAt(Position(0, 0))->getId();
    RealTimeArbiter arbiter;

    check(arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0), "the motion is accepted");
    check(board.isEmpty(Position(0, 0)), "the origin is vacated at launch");
    check(board.isEmpty(Position(0, 1)), "the destination is still empty");
    check(arbiter.hasActiveMotions(), "the motion is registered");
    check(board.findPieceById(rookId) != nullptr, "the piece still exists while flying");
    check(board.findPieceById(rookId)->getState() == PieceState::Moving, "state is Moving");
}

TEST_CASE("a launched piece is flagged Moving") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    const int rookId = board.getPieceAt(Position(0, 0))->getId();
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    check(arbiter.isPieceMoving(rookId), "the arbiter tracks the flying piece by id");
    check(!arbiter.isMoving(board, Position(0, 0)), "an empty origin is not 'moving'");
}

TEST_CASE("a piece in flight refuses a second motion from its empty origin") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    check(!arbiter.startMotion(board, Position(0, 0), Position(0, 2), 0), "nothing left on the origin to launch");
}

TEST_CASE("nothing lands before the arrival time") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    arbiter.advanceTo(999, board);
    check(board.isEmpty(Position(0, 0)), "origin stays empty mid-flight");
    check(board.isEmpty(Position(0, 1)), "at t=999 the piece has not landed");
    check(arbiter.hasActiveMotions(), "the motion is still in flight");
}

TEST_CASE("the piece lands exactly on time and returns to Idle") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    arbiter.advanceTo(1000, board);
    check(board.isEmpty(Position(0, 0)), "the source is vacated");
    check(!board.isEmpty(Position(0, 1)), "the destination is filled");
    check(board.getPieceAt(Position(0, 1))->getState() == PieceState::Idle, "the piece is Idle again");
    check(!arbiter.hasActiveMotions(), "the motion is dropped");
}

TEST_CASE("arriving on an enemy reports a capture event") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 1);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    auto early = arbiter.advanceTo(500, board);
    check(early.captures.empty(), "no capture before arrival");

    auto events = arbiter.advanceTo(1000, board);
    check(events.captures.size() == 1, "exactly one capture");
    check(events.captures[0].capturedKind == PieceKind::King, "the event names the kind");
    check(events.captures[0].capturedColor == PieceColor::Black, "the event names the colour");
    check(events.captures[0].cell == Position(0, 1), "the event names the cell");
}

TEST_CASE("arriving on an empty cell reports nothing") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    auto events = arbiter.advanceTo(1000, board);
    check(events.captures.empty(), "a quiet arrival raises no capture event");
}

TEST_CASE("two pieces fly at once") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 0);
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);
    arbiter.startMotion(board, Position(2, 0), Position(2, 1), 0);
    check(arbiter.hasActiveMotions(), "both are in flight");

    arbiter.advanceTo(1000, board);
    check(!board.isEmpty(Position(0, 1)), "the first landed");
    check(!board.isEmpty(Position(2, 1)), "the second landed");
    check(!arbiter.hasActiveMotions(), "both are cleared");
}

TEST_CASE("a longer move lands later than a shorter one") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);  // 2 cells -> 2000ms
    put(board, PieceColor::Black, PieceKind::Rook, 2, 0);  // 1 cell  -> 1000ms
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 2), 0);
    arbiter.startMotion(board, Position(2, 0), Position(2, 1), 0);

    arbiter.advanceTo(1000, board);
    check(!board.isEmpty(Position(2, 1)), "the short move has landed");
    check(board.isEmpty(Position(0, 0)), "the long move has left its origin");
    check(board.isEmpty(Position(0, 2)), "the long move has NOT landed yet");
    check(arbiter.hasActiveMotions(), "the long move is still flying");

    arbiter.advanceTo(2000, board);
    check(!board.isEmpty(Position(0, 2)), "the long move lands on its own schedule");
}

TEST_CASE("one large advance resolves every pending arrival") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 0);
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);
    arbiter.startMotion(board, Position(2, 0), Position(2, 1), 0);
    arbiter.advanceTo(999999, board);

    check(!board.isEmpty(Position(0, 1)) && !board.isEmpty(Position(2, 1)), "both landed in one jump");
    check(!arbiter.hasActiveMotions(), "nothing is left in flight");
}

TEST_CASE("two pieces racing to one cell do not stack") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);
    arbiter.startMotion(board, Position(2, 2), Position(0, 1), 0);
    arbiter.advanceTo(2000, board);

    check(board.getPieceAt(Position(0, 1))->getKind() == PieceKind::Rook, "the winner holds the cell");
    check(!board.isEmpty(Position(2, 2)), "the loser stayed home");
    check(board.getPieceAt(Position(2, 2))->getState() == PieceState::Idle,
          "the loser is Idle again, not stuck as Moving forever");
}

TEST_CASE("a motion from an empty cell is refused") {
    Board board(3, 3);
    RealTimeArbiter arbiter;
    check(!arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0), "nothing to launch");
    check(!arbiter.hasActiveMotions(), "no motion was registered");
}

TEST_CASE("isMoving on an empty cell is false, not a crash") {
    Board board(3, 3);
    RealTimeArbiter arbiter;
    check(!arbiter.isMoving(board, Position(1, 1)), "an empty cell is not moving");
}

TEST_CASE("a moving piece cannot be captured on its vacated origin") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Rook, 1, 0);
    const int whiteId = board.getPieceAt(Position(0, 0))->getId();

    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 2), 0);
    arbiter.startMotion(board, Position(1, 0), Position(0, 0), 0);

    auto mid = arbiter.advanceTo(1000, board);
    check(mid.captures.empty(), "landing on a vacated origin is not a capture");
    check(!board.isEmpty(Position(0, 0)), "black occupies the origin");
    check(board.getPieceAt(Position(0, 0))->getColor() == PieceColor::Black, "black holds origin");
    check(arbiter.isPieceMoving(whiteId), "white is still flying");

    arbiter.advanceTo(2000, board);
    check(!board.isEmpty(Position(0, 2)), "white lands on its destination");
    check(board.getPieceAt(Position(0, 2))->getColor() == PieceColor::White, "white survived");
    check(board.getPieceAt(Position(0, 0))->getColor() == PieceColor::Black, "black still on origin");
}

TEST_CASE("a moving piece can be captured at its live path cell") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0); // 0,0 -> 0,2: on (0,1) from t=1000
    put(board, PieceColor::Black, PieceKind::Rook, 1, 1); // starts later, arrives on (0,1) at t=1500
    const int whiteId = board.getPieceAt(Position(0, 0))->getId();

    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 2), 0);
    arbiter.startMotion(board, Position(1, 1), Position(0, 1), 500);

    auto events = arbiter.advanceTo(1500, board);
    check(events.captures.size() == 1, "mid-path collision captures");
    check(events.captures[0].capturedPieceId == whiteId, "the flying white rook is taken");
    check(events.captures[0].cell == Position(0, 1), "captured at the live path cell");
    check(!board.isEmpty(Position(0, 1)), "black holds the collision cell");
    check(board.getPieceAt(Position(0, 1))->getColor() == PieceColor::Black, "black survived");
    check(board.findPieceById(whiteId) == nullptr, "white is gone");
}
