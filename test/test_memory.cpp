#include "sw/memory.hpp"
#include "utils.hpp"

void run_tests()
{
    // Basic alignment
    ASSERT_EQ(sw::aligned_size(1, 4), 4);
    ASSERT_EQ(sw::aligned_size(4, 4), 4);
    ASSERT_EQ(sw::aligned_size(5, 4), 8);

    // Template versions
    ASSERT_EQ(sw::aligned_size<8>(1), 8);
    ASSERT_EQ(sw::aligned_size<8>(8), 8);
    ASSERT_EQ(sw::aligned_size<8>(9), 16);

    // Type-based alignment
    struct alignas(16) S
    {
        char c;
    };
    ASSERT_EQ((sw::aligned_size<S, 32>()), 32);
}

TEST_MAIN
