#pragma once

#include <print>
#include <cstdlib>

// 간단한 테스트 상태 관리
static int g_fails = 0;

// 값 비교 매크로
#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            std::println(stderr, "[FAIL] {} != {}", #a, #b); \
            std::println(stderr, "       Value: {} != {}", (a), (b)); \
            std::println(stderr, "       Loc  : {}:{}", __FILE__, __LINE__); \
            g_fails++; \
        } \
    } while (0)

// 조건 확인 매크로
#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::println(stderr, "[FAIL] {} is false", #cond); \
            std::println(stderr, "       Loc  : {}:{}", __FILE__, __LINE__); \
            g_fails++; \
        } \
    } while (0)

// 메인 함수 생성 매크로
#define TEST_MAIN \
    int main() { \
        std::println("[TEST] Running {} ...", __FILE__); \
        run_tests(); \
        if (g_fails > 0) { \
            std::println("[FAIL] {} assertion(s) failed.", g_fails); \
            return 1; \
        } \
        std::println("[PASS] All assertions passed."); \
        return 0; \
    }
