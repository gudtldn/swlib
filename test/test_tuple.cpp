#include "sw/tuple.hpp"

#include <variant>
#include "utils.hpp"

void run_tests()
{
    // tuple_cat_t
    using T1 = std::tuple<int>;
    using T2 = std::tuple<float>;
    static_assert(std::is_same_v<sw::tuple_cat_t<T1, T2>, std::tuple<int, float>>);

    // rebind_t
    using MyTuple = std::tuple<int, float>;
    using MyVariant = sw::rebind_t<MyTuple, std::variant>;
    static_assert(std::is_same_v<MyVariant, std::variant<int, float>>);

    // flatten_tuple_t
    using Nested = std::tuple<int, std::tuple<float, double>>;
    using Flat = sw::flatten_tuple_t<Nested>;
    static_assert(std::is_same_v<Flat, std::tuple<int, float, double>>);

    // unpack_types
    constexpr int count = sw::unpack_types<std::tuple<int, float, double>>(
        []<typename... Args>()
        {
            return sizeof...(Args);
        }
    );

    static_assert(count == 3);
    ASSERT_EQ(count, 3);
}

TEST_MAIN
