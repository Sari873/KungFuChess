#pragma once
// ---------------------------------------------------------------------------
// Minimal test harness. No external framework, no build system.
//
// Each test file declares its cases with TEST_CASE(name) and they register
// themselves automatically at static-init time - TestMain.cpp never has to
// know they exist. Adding a new test file means adding one .cpp; nothing
// else changes.
//
//   TEST_CASE("Rook slides right") {
//       check(legal(board, a, b), "slides right");
//   }
// ---------------------------------------------------------------------------

#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct TestCase {
    std::string suite;
    std::string name;
    std::function<void()> body;
};

// Meyers singleton: safe under static-init ordering, unlike a plain global.
inline std::vector<TestCase>& testRegistry() {
    static std::vector<TestCase> registry;
    return registry;
}

inline int& passedCount() { static int n = 0; return n; }
inline int& failedCount() { static int n = 0; return n; }

// Name of the test currently running, used to label failures.
inline std::string& currentTest() { static std::string s; return s; }

struct TestRegistrar {
    TestRegistrar(const std::string& suite, const std::string& name, std::function<void()> body) {
        testRegistry().push_back(TestCase{suite, name, std::move(body)});
    }
};

// TEST_SUITE_NAME must be #defined at the top of each test file.
#define TEST_CASE_IMPL2(suite, name, fn, reg)                       \
    static void fn();                                               \
    static TestRegistrar reg(suite, name, fn);                      \
    static void fn()

#define TEST_CASE_IMPL(suite, name, counter) \
    TEST_CASE_IMPL2(suite, name, testFn_##counter, testReg_##counter)

#define TEST_CASE_EXPAND(suite, name, counter) TEST_CASE_IMPL(suite, name, counter)

#define TEST_CASE(name) TEST_CASE_EXPAND(TEST_SUITE_NAME, name, __LINE__)

// --------------------------------- asserts ---------------------------------

inline void check(bool condition, const std::string& label) {
    if (condition) {
        ++passedCount();
    } else {
        ++failedCount();
        std::cout << "    FAIL  " << label << "\n";
    }
}

inline void checkEq(const std::string& actual, const std::string& expected, const std::string& label) {
    if (actual == expected) {
        ++passedCount();
    } else {
        ++failedCount();
        std::cout << "    FAIL  " << label << "\n"
                  << "          expected: \"" << expected << "\"\n"
                  << "          actual:   \"" << actual << "\"\n";
    }
}

inline void checkEq(long long actual, long long expected, const std::string& label) {
    if (actual == expected) {
        ++passedCount();
    } else {
        ++failedCount();
        std::cout << "    FAIL  " << label << "\n"
                  << "          expected: " << expected << "\n"
                  << "          actual:   " << actual << "\n";
    }
}
