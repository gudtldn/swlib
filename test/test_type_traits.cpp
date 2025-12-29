#include "sw/type_traits.hpp"
#include "utils.hpp"

void free_function(int, float)
{
}

void run_tests()
{
    // Free function
    using F1 = sw::function_traits<decltype(free_function)>;
    static_assert(F1::arity == 2);
    static_assert(std::is_same_v<F1::return_type, void>);

    // Lambda
    auto l = [](int a) { return a * 2.0; };
    using L = sw::function_traits<decltype(l)>;
    static_assert(L::arity == 1);
    static_assert(std::is_same_v<L::return_type, double>);

    // Member function
    struct S
    {
        void mem(int) const
        {
        }
    };
    using M = sw::function_traits<decltype(&S::mem)>;
    static_assert(M::arity == 1);
}

TEST_MAIN
