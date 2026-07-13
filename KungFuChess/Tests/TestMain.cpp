// Runs every test case registered by the test files. Adding a new
// test_*.cpp requires no change here - the cases register themselves.

#include "TestHarness.h"
#include <algorithm>
#include <iostream>
#include <string>

int main() {
    auto& cases = testRegistry();

    // Group by suite so the output reads layer by layer.
    std::stable_sort(cases.begin(), cases.end(),
                     [](const TestCase& a, const TestCase& b) {
                         return a.suite < b.suite;
                     });

    std::string currentSuite;

    for (const TestCase& test : cases) {
        if (test.suite != currentSuite) {
            currentSuite = test.suite;
            std::cout << "\n[" << currentSuite << "]\n";
        }

        int failedBefore = failedCount();
        currentTest() = test.name;
        test.body();

        bool passed = (failedCount() == failedBefore);
        std::cout << (passed ? "  ok    " : "  FAILED ") << test.name << "\n";
    }

    std::cout << "\n=================================================\n";
    std::cout << "cases: " << cases.size()
              << "   assertions passed: " << passedCount()
              << "   failed: " << failedCount() << "\n";
    std::cout << (failedCount() == 0 ? "ALL TESTS PASSED" : "THERE ARE FAILURES") << "\n";
    std::cout << "=================================================\n";

    return failedCount() == 0 ? 0 : 1;
}
