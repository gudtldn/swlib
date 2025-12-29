#include <string>
#include <array>
#include <iostream>

#include "sw/function.hpp"
#include "utils.hpp"

// Free function for testing
int add(int a, int b) { return a + b; }

struct Functor
{
    int operator()(int a, int b) const { return a * b; }
};

void run_tests()
{
    // 1. Basic Construction & Invocation
    {
        sw::function<int(int, int)> f;
        ASSERT_TRUE(!f); // Empty check

        f = add;
        ASSERT_TRUE(f);
        ASSERT_EQ(f(2, 3), 5);

        f = Functor{};
        ASSERT_EQ(f(3, 4), 12);

        f = [](int a, int b) { return a - b; };
        ASSERT_EQ(f(10, 3), 7);
    }

    // 2. SBO (Small Buffer Optimization) Check
    {
        int x = 100;
        // Capture creates a small object (sizeof(int) < sbo_buffer_size)
        sw::function<int()> f = [x]() { return x; };
        ASSERT_EQ(f(), 100);

        // Copy
        auto f2 = f;
        ASSERT_EQ(f2(), 100);

        // Move
        auto f3 = std::move(f);
        ASSERT_EQ(f3(), 100);
        ASSERT_TRUE(!f); // f should be empty after move
    }

    // 3. Heap Allocation (Large Object) Check
    {
        struct LargeObject
        {
            std::array<char, 128> data;
            int val;
            int operator()() const { return val; }
        };

        LargeObject large;
        large.val = 999;

        sw::function<int()> f = large;
        ASSERT_EQ(f(), 999);

        // Copy (Deep copy verification)
        auto f2 = f;
        ASSERT_EQ(f2(), 999);

        // Move
        auto f3 = std::move(f);
        ASSERT_EQ(f3(), 999);
        ASSERT_TRUE(!f);
    }

    // 4. Nullptr Handling
    {
        sw::function<void()> f = []
        {
        };
        ASSERT_TRUE(f);

        f = nullptr;
        ASSERT_TRUE(!f);
        ASSERT_TRUE(f == nullptr);
    }

    // 5. std::bad_function_call
    {
        sw::function<void()> f;
        bool caught = false;
        try
        {
            f();
        }
        catch (const std::bad_function_call&)
        {
            caught = true;
        }
        ASSERT_TRUE(caught);
    }

    // 6. Return type conversion
    {
        // lambda returns int, function expects double -> should convert
        sw::function<double()> f = []() -> int { return 42; };
        ASSERT_EQ(f(), 42.0);
    }
}

TEST_MAIN
