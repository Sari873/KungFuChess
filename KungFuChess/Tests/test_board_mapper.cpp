#define TEST_SUITE_NAME "BoardMapper"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../Input/BoardMapper.h"

TEST_CASE("the first pixel maps to the first cell") {
    Board board(3, 3);
    Position out;
    check(BoardMapper::pixelsToCell(0, 0, board, out) && out == Position(0, 0), "(0,0) pixel is (0,0) cell");
}

TEST_CASE("a cell owns its last pixel") {
    Board board(3, 3);
    Position out;
    check(BoardMapper::pixelsToCell(99, 99, board, out) && out == Position(0, 0), "pixel 99 is still cell 0");
    check(BoardMapper::pixelsToCell(100, 0, board, out) && out == Position(0, 1), "pixel 100 crosses to cell 1");
    check(BoardMapper::pixelsToCell(0, 100, board, out) && out == Position(1, 0), "and the same vertically");
}

TEST_CASE("x is the column and y is the row") {
    // Swapping these is the single easiest bug to write here.
    Board board(3, 3);
    Position out;
    check(BoardMapper::pixelsToCell(250, 150, board, out), "the click lands on the board");
    check(out == Position(1, 2), "x=250 -> col 2, y=150 -> row 1");
}

TEST_CASE("the last pixel of the board maps to the last cell") {
    Board board(3, 3);
    Position out;
    check(BoardMapper::pixelsToCell(299, 299, board, out) && out == Position(2, 2), "pixel 299 is cell 2");
}

TEST_CASE("one pixel past the edge is off the board") {
    Board board(3, 3);
    Position out;
    check(!BoardMapper::pixelsToCell(300, 0, board, out), "just past the right edge");
    check(!BoardMapper::pixelsToCell(0, 300, board, out), "just past the bottom edge");
}

TEST_CASE("negative pixels are off the board") {
    Board board(3, 3);
    Position out;
    check(!BoardMapper::pixelsToCell(-1, 50, board, out), "negative x");
    check(!BoardMapper::pixelsToCell(50, -1, board, out), "negative y");
}

TEST_CASE("a non-square board has different limits per axis") {
    Board board(2, 4); // 400 wide, 200 tall
    Position out;
    check(BoardMapper::pixelsToCell(350, 150, board, out) && out == Position(1, 3), "the far corner is valid");
    check(!BoardMapper::pixelsToCell(350, 250, board, out), "past the short axis is off the board");
    check(!BoardMapper::pixelsToCell(450, 150, board, out), "past the long axis is off the board");
}
