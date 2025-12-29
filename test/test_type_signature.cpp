#include "sw/type_signature.hpp"
#include "utils.hpp"

namespace my_ns
{
struct MyStruct
{
};
}

void run_tests()
{
    // Basic types
    ASSERT_EQ(sw::type_name<int>(), "int");

    // Namespaced types
    ASSERT_EQ(sw::type_name<my_ns::MyStruct>(), "MyStruct");
    ASSERT_EQ(sw::full_type_name<my_ns::MyStruct>(), "my_ns::MyStruct");

    // CV-qualifiers should be stripped for name()
    ASSERT_EQ(sw::type_name<const int volatile&>(), "int");

    ASSERT_EQ(sw::type_name<const int volatile* const** const volatile*** const&>(), "int");
}

TEST_MAIN
