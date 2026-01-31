#pragma once

#include <string_view>
#include <concepts>

#include "sw/types.hpp"


namespace sw
{
namespace internal
{
// FNV-1a 64-bit Hash Constants
constexpr u64 fnv_offset_basis = 0xcbf29ce484222325ULL;
constexpr u64 fnv_prime = 0x100000001b3ULL;

/** FNV-1a 해시 알고리즘 구현부 */
template <typename CharType>
constexpr u64 fnv1a_impl(std::basic_string_view<CharType> view) noexcept
{
    // 1바이트 문자(char, char8_t)만 지원
    static_assert(sizeof(CharType) == 1, "Only 1-byte character types are supported.");

    u64 hash = fnv_offset_basis;
    for (const CharType c : view)
    {
        const u8 byte = static_cast<u8>(c);
        hash ^= byte;
        hash *= fnv_prime;
    }
    return hash;
}
}

/** 문자열을 64비트 정수 해시값으로 변환합니다. (FNV-1a 알고리즘) */
template <typename StringType>
    requires std::constructible_from<std::string_view, StringType>
constexpr u64 fnv1a(const StringType& str) noexcept
{
    return internal::fnv1a_impl(std::string_view{ str });
}

/** 문자열 리터럴(배열)을 64비트 정수 해시값으로 변환합니다. (FNV-1a 알고리즘) */
template <typename CharType, usize N>
constexpr u64 fnv1a(const CharType(&str)[N]) noexcept
{
    // 리터럴 크기 N에는 널 문자가 포함되어 있으므로 N-1 길이만 사용
    return internal::fnv1a_impl(std::basic_string_view<CharType>{ str, N - 1 });
}

namespace literals
{
/** 문자열 뒤에 _hash를 붙여 즉시 해시값으로 변환합니다. */
constexpr u64 operator""_fnv1a(const char* str, usize len) noexcept
{
    return internal::fnv1a_impl(std::string_view{ str, len });
}
}
} // namespace sw
