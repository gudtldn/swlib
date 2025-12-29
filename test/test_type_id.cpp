#include "sw/type_id.hpp"
#include <format>
#include "utils.hpp"

// sw::type_id를 출력할 때 타입 이름을 출력하도록 formatter 특수화
template <>
struct std::formatter<sw::type_id> : std::formatter<std::string_view>
{
    auto format(const sw::type_id& id, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(id.name(), ctx);
    }
};

void run_tests()
{
    auto id1 = sw::type_id::get<int>();
    auto id2 = sw::type_id::get<int>();
    auto id3 = sw::type_id::get<float>();

    ASSERT_EQ(id1, id2);
    ASSERT_TRUE(id1 != id3);
    ASSERT_TRUE(id1.is_valid());

    // Name check
    ASSERT_EQ(sw::type_id::get<const int&>().name(), id1.name());
}

TEST_MAIN
