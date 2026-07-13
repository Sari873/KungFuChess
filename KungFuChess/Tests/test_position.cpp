#define TEST_SUITE_NAME "Position"
#include "TestHarness.h"
#include "../Model/Position.h"

TEST_CASE("stores its coordinates") {
    Position p(2, 3);
    check(p.getRow() == 2, "row is kept");
    check(p.getCol() == 3, "col is kept");
}

TEST_CASE("compares by value") {
    check(Position(2, 3) == Position(2, 3), "identical coordinates are equal");
    check(Position(2, 3) != Position(2, 4), "a different col is not equal");
    check(Position(2, 3) != Position(3, 3), "a different row is not equal");
}

TEST_CASE("row and col are not interchangeable") {
    check(Position(2, 3) != Position(3, 2), "(2,3) is not (3,2)");
}

TEST_CASE("default constructs to the origin") {
    check(Position() == Position(0, 0), "default is (0,0)");
}

TEST_CASE("accepts negative coordinates without complaint") {
    // Position is a value object: bounds-checking belongs to Board, not here.
    Position p(-1, -5);
    check(p.getRow() == -1 && p.getCol() == -5, "negatives are stored as given");
}
