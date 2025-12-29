#pragma once

#include <concepts>
#include <type_traits>
#include <utility>


namespace sw
{
// =========================================================================
// 기본 유틸리티
// =========================================================================

// static_assert 실패 유도용
template <typename T>
concept always_false = false;

// T가 Ts... 중 하나와 일치하는지 확인
template <typename T, typename... Ts>
concept is_any_of = (std::same_as<T, Ts> || ...);

// T가 Ts... 중 하나와 일치하는지 확인 (decay 적용: const/ref 제거 후 비교)
template <typename T, typename... Ts>
concept is_any_of_decayed = is_any_of<std::decay_t<T>, std::decay_t<Ts>...>;

// T가 PrimaryTemplate<Args...> 형태의 특수화인지 확인
template <typename T, template <typename...> typename PrimaryTemplate>
concept is_specialization_of = requires
{
    []<typename... Args>(const PrimaryTemplate<Args...>&)
    {
    }(std::declval<T>());
};

// =========================================================================
// std concepts 보완
// =========================================================================

// Enum concept
template <typename T>
concept enum_type = std::is_enum_v<T>;

// Number concept (integral or floating point)
template <typename T>
concept number_type = std::integral<T> || std::floating_point<T>;
} // namespace sw
