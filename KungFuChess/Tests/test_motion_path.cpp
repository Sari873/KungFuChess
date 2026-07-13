#define TEST_SUITE_NAME "MotionPath"
#include "TestHarness.h"
#include "../RealTime/MotionPath.h"

TEST_CASE("one cell costs one second") {
    MotionPath path(Position(0, 0), Position(0, 1), 0);
    checkEq(path.getDurationMs(), 1000, "duration");
    checkEq(path.getArrivalMs(), 1000, "arrival");
}

TEST_CASE("distance scales the duration") {
    checkEq(MotionPath(Position(0, 0), Position(0, 3), 0).getDurationMs(), 3000, "three cells");
    checkEq(MotionPath(Position(0, 0), Position(7, 0), 0).getDurationMs(), 7000, "seven cells");
}

TEST_CASE("a diagonal is charged by its longest axis") {
    // A 2x2 diagonal is 2 cells of travel, not 4.
    checkEq(MotionPath(Position(0, 0), Position(2, 2), 0).getDurationMs(), 2000, "diagonal");
}

TEST_CASE("a knight's L is charged by its longest leg") {
    checkEq(MotionPath(Position(0, 0), Position(2, 1), 0).getDurationMs(), 2000, "L jump");
}

TEST_CASE("arrival is exact, not approximate") {
    MotionPath path(Position(0, 0), Position(0, 1), 0);
    check(!path.hasArrived(999), "not one tick early");
    check(path.hasArrived(1000), "exactly on time");
    check(path.hasArrived(1001), "and after");
    check(path.hasArrived(999999), "stays arrived forever");
}

TEST_CASE("the start time offsets the arrival") {
    MotionPath path(Position(0, 0), Position(0, 1), 5000);
    checkEq(path.getArrivalMs(), 6000, "arrival is start + duration");
    check(!path.hasArrived(5999), "a late-started motion is not retroactively arrived");
    check(path.hasArrived(6000), "it lands on its own schedule");
}

TEST_CASE("progress runs from zero to one") {
    MotionPath path(Position(0, 0), Position(0, 1), 0);
    check(path.progress(0) == 0.0, "starts at 0");
    check(path.progress(500) > 0.49 && path.progress(500) < 0.51, "about half way at 500ms");
    check(path.progress(1000) == 1.0, "ends at 1");
}

TEST_CASE("progress is clamped at both ends") {
    MotionPath path(Position(0, 0), Position(0, 1), 0);
    check(path.progress(99999) == 1.0, "never exceeds 1");
    check(path.progress(-500) == 0.0, "never drops below 0");
}

TEST_CASE("remembers its endpoints") {
    MotionPath path(Position(1, 2), Position(3, 4), 0);
    check(path.getSrc() == Position(1, 2), "source");
    check(path.getDst() == Position(3, 4), "destination");
}
