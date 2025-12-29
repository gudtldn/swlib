#include "sw/hash.hpp"
#include "utils.hpp"

void run_tests()
{
    using namespace sw::literals;

    // Compile-time
    constexpr sw::u64 h1 = sw::fnv1a("hello");
    constexpr sw::u64 h2 = "hello"_fnv1a;
    static_assert(h1 == h2);

    // Runtime
    std::string s = "world";
    sw::u64 h3 = sw::fnv1a(s);
    sw::u64 h4 = "world"_fnv1a;
    ASSERT_EQ(h3, h4);

    // Known value for "a"
    ASSERT_EQ("a"_fnv1a, 0xaf63dc4c8601ec8cULL);
}

TEST_MAIN
