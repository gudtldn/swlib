#pragma once

#include <bit>
#include <concepts>
#include <cassert>

#include "sw/types.hpp"


namespace sw
{
/**
 * 특정 값(size)을 지정된 정렬(alignment) 크기로 올림(round up)합니다.
 * @param size 원본 크기
 * @param alignment 정렬 단위 (반드시 2의 거듭제곱이어야 함)
 * @return 정렬된 크기 (usize)
 */
constexpr usize aligned_size(usize size, usize alignment)
{
    assert(std::has_single_bit(alignment) && "Alignment must be power of 2");
    return (size + alignment - 1) & ~(alignment - 1);
}

/**
 * 특정 값(size)을 지정된 정렬(Alignment) 크기로 올림(round up)합니다.
 * @tparam Alignment 정렬 단위 (2의 거듭제곱이어야 함)
 * @param size 원본 크기
 */
template <usize Alignment>
    requires (std::has_single_bit(Alignment))
constexpr usize aligned_size(usize size)
{
    return (size + Alignment - 1) & ~(Alignment - 1);
}

/**
 * 특정 타입(T)의 크기를 지정된 정렬(Alignment) 크기로 올림(round up)합니다.
 * @tparam T 대상 타입
 * @tparam Alignment 정렬 단위 (2의 거듭제곱이어야 함)
 */
template <typename T, usize Alignment>
    requires (std::has_single_bit(Alignment))
constexpr usize aligned_size()
{
    return aligned_size<Alignment>(sizeof(T));
}
} // namespace sw
