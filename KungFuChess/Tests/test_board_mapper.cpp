#define TEST_SUITE_NAME "BoardMapper"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../GameConstants.h"
#include "../Input/BoardMapper.h"
#include "../Presentation/DisplayTypes.h"

TEST_CASE("the first pixel maps to the first cell") {
    CellCoord out;
    check(BoardMapper::pixelsToCell(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral,
                                    Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out)
              && out == CellCoord(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral),
          "(0,0) pixel is (0,0) cell");
}

TEST_CASE("a cell owns its last pixel") {
    CellCoord out;
    check(BoardMapper::pixelsToCell(Kfc::Test::kPixelJustInsideCell, Kfc::Test::kPixelJustInsideCell,
                                    Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out)
              && out == CellCoord(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral),
          "pixel 99 is still cell 0");
    check(BoardMapper::pixelsToCell(Kfc::Test::kPixelCellBoundary, Kfc::Grid::kNeutral,
                                    Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out)
              && out == CellCoord(Kfc::Grid::kNeutral, Kfc::Grid::kForward),
          "pixel 100 crosses to cell 1");
    check(BoardMapper::pixelsToCell(Kfc::Grid::kNeutral, Kfc::Test::kPixelCellBoundary,
                                    Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out)
              && out == CellCoord(Kfc::Grid::kForward, Kfc::Grid::kNeutral),
          "and the same vertically");
}

TEST_CASE("x is the column and y is the row") {
    CellCoord out;
    check(BoardMapper::pixelsToCell(Kfc::Test::kDiagonalClickX, Kfc::Test::kDiagonalClickY,
                                    Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out),
          "the click lands on the board");
    check(out == CellCoord(Kfc::Grid::kForward, Kfc::Grid::kPawnDoubleStepCells),
          "x=250 -> col 2, y=150 -> row 1");
}

TEST_CASE("the last pixel of the board maps to the last cell") {
    CellCoord out;
    check(BoardMapper::pixelsToCell(Kfc::Test::kPixelLastInCell, Kfc::Test::kPixelLastInCell,
                                    Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out)
              && out == CellCoord(Kfc::Test::kLastCellIndex, Kfc::Test::kLastCellIndex),
          "pixel 299 is cell 2");
}

TEST_CASE("one pixel past the edge is off the board") {
    CellCoord out;
    check(!BoardMapper::pixelsToCell(Kfc::Test::kPixelPastBoardEdge, Kfc::Grid::kNeutral,
                                     Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out),
          "just past the right edge");
    check(!BoardMapper::pixelsToCell(Kfc::Grid::kNeutral, Kfc::Test::kPixelPastBoardEdge,
                                     Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out),
          "just past the bottom edge");
}

TEST_CASE("negative pixels are off the board") {
    CellCoord out;
    check(!BoardMapper::pixelsToCell(Kfc::Test::kNegativePixel, Kfc::Test::kProbePixelMid,
                                     Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out),
          "negative x");
    check(!BoardMapper::pixelsToCell(Kfc::Test::kProbePixelMid, Kfc::Test::kNegativePixel,
                                     Kfc::Test::kTinyBoardExtent, Kfc::Test::kTinyBoardExtent, out),
          "negative y");
}

TEST_CASE("a non-square board has different limits per axis") {
    CellCoord out;
    check(BoardMapper::pixelsToCell(Kfc::Test::kNonSquareProbeX, Kfc::Test::kNonSquareProbeYValid,
                                    Kfc::Test::kSmallBoardRows, Kfc::Test::kWideBoardCols, out)
              && out == CellCoord(Kfc::Grid::kForward, Kfc::Test::kWideBoardCols - Kfc::Grid::kForward),
          "the far corner is valid");
    check(!BoardMapper::pixelsToCell(Kfc::Test::kNonSquareProbeX, Kfc::Test::kNonSquareProbeYPast,
                                     Kfc::Test::kSmallBoardRows, Kfc::Test::kWideBoardCols, out),
          "past the short axis is off the board");
    check(!BoardMapper::pixelsToCell(Kfc::Test::kNonSquareProbeXPast, Kfc::Test::kNonSquareProbeYValid,
                                     Kfc::Test::kSmallBoardRows, Kfc::Test::kWideBoardCols, out),
          "past the long axis is off the board");
}
