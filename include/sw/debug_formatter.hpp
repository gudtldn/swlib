#pragma once
#include <format>

#include "type_signature.hpp"


namespace sw
{
namespace internal
{
template <typename T>
void debug_print_impl(const T& value, std::format_context& ctx)
{
    // TODO: 추후 C++26 리플렉션이 들어오면 여기서 멤버 순회
    // 일단 "TypeName @ Address" 형식으로 출력
    if constexpr (std::is_pointer_v<T>)
    {
        std::format_to(ctx.out(), "{}* @ {}", sw::type_name<T>(), static_cast<void*>(value));
    }
    else
    {
        std::format_to(ctx.out(), "{} @ {}", sw::type_name<T>(), static_cast<const void*>(&value));
    }
}
} // namespace internal


template <typename T>
struct debug_formatter_base
{
    // ReSharper disable once CppMemberFunctionMayBeStatic
    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();

        // 포맷 지정자가 '?'로 시작하는지 확인
        if (it != ctx.end() && *it == '?')
        {
            ++it; // '?' 소비
        }
        else
        {
            // '?'가 없으면 에러 발생 (일반 Display Formatter 미구현일 때)
            throw std::format_error("Format specifier missing '?'. This type only supports {:?} (Debug style).");
        }

        if (it != ctx.end() && *it != '}')
        {
            throw std::format_error("Debug formatter does not support additional modifiers yet.");
        }
        return it;
    }

    auto format(const T& obj, std::format_context& ctx) const
    {
        internal::debug_print_impl(obj, ctx);
        return ctx.out();
    }
};
} // namespace sw

#define SW_DERIVE_DEBUG(Type) \
    template <> \
    struct std::formatter<Type> : sw::debug_formatter_base<Type> {}
