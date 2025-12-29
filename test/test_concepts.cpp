#include <vector>
#include <string>
#include "sw/concepts.hpp"
#include "utils.hpp"

void run_tests()
{
    // 1. is_any_of
    static_assert(sw::is_any_of<int, double, int, char>);
    static_assert(!sw::is_any_of<void, double, int, char>);

    // 2. is_specialization_of
    static_assert(sw::is_specialization_of<std::vector<int>, std::vector>);
    static_assert(!sw::is_specialization_of<int, std::vector>);

    // 3. number_type
    static_assert(sw::number_type<int>);
    static_assert(sw::number_type<float>);
    static_assert(!sw::number_type<std::string>);

    // 4. enum_type
    enum class Color { Red };
    static_assert(sw::enum_type<Color>);
    static_assert(!sw::enum_type<int>);

    ASSERT_TRUE(true); // Runtime check placeholder
}

TEST_MAIN
