#pragma once

#include <tuple>
#include <type_traits>
#include <utility>


namespace sw
{
namespace internal
{
/**
 * TupleLike에 포함된 모든 타입을 템플릿 파라미터 팩 `Ts...`로 추출(unpack)합니다.
 */
template <typename Tuple>
struct unpack_impl;

template <template <typename...> typename TupleLike, typename... Ts>
struct unpack_impl<TupleLike<Ts...>>
{
    template <typename Fn>
    static constexpr auto apply(Fn&& func)
    {
        return std::forward<Fn>(func).template operator()<Ts...>();
    }
};

template <typename R, typename... Ts>
struct unpack_impl<R(Ts...)>
{
    template <typename Fn>
    static constexpr auto apply(Fn&& func)
    {
        return std::forward<Fn>(func).template operator()<Ts...>();
    }
};

/**
 * 템플릿 파라미터 팩을 다른 템플릿 컨테이너로 리바인딩(rebind)합니다.
 * @tparam Tuple 템플릿 파라미터 팩을 추출할 원본 템플릿 타입 (예: `std::tuple<int, float>`)
 */
template <typename Tuple>
struct rebind_impl;

template <template <typename...> typename TupleLike, typename... Ts>
struct rebind_impl<TupleLike<Ts...>>
{
    template <template <typename...> typename Target>
    using to = Target<Ts...>;
};

template <typename R, typename... Ts>
struct rebind_impl<R(Ts...)>
{
    template <template <typename...> typename Target>
    using to = Target<Ts...>;
};

/** 여러 튜플 타입을 하나로 합친 타입을 반환합니다. */
template <typename... Tuples>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Tuples>()...));

template <template <typename...> typename ResultTuple, typename T>
struct flatten_impl
{
    // 튜플이 아니면 그대로 튜플로 감싸서 반환
    using type = ResultTuple<T>;
};

template <template <typename...> typename ResultTuple, template <typename...> typename InputTuple, typename... Ts>
struct flatten_impl<ResultTuple, InputTuple<Ts...>>
{
    // 내부 요소들을 flatten 하고, 그 결과 타입들을 tuple_cat으로 합침
    using type = tuple_cat_t<typename flatten_impl<ResultTuple, Ts>::type...>;
};
} // namespace internal

/**
 * 튜플이나 함수 시그니처에서 타입들을 추출하여 람다의 템플릿 인자로 전달합니다.
 * @note std::apply는 '값'을 풀지만, sw::unpack_types은 '타입'을 풉니다.
 *
 * @tparam TupleLike 타입들을 추출할 TupleLike (ex: std::tuple<...>)
 * @tparam Fn 템플릿 `operator()`를 가진 제네릭 Lambda 또는 Functor 타입
 * @param func 추출된 타입들을 템플릿 인자로 받아 호출될 객체
 * @return `func`를 호출한 결과값을 그대로 반환
 *
 * @code
 * using MyTuple = std::tuple<int, float>;
 * sw::unpack_types<MyTuple>([]<typename... Args>()
 * {
 *     // Args...는 int, float가 됨
 *     return sizeof...(Args); // 2를 반환
 * });
 * @endcode
 */
template <typename TupleLike, typename Fn>
constexpr auto unpack_types(Fn&& func)
{
    return internal::unpack_impl<TupleLike>::apply(std::forward<Fn>(func));
}

/**
 * 템플릿 컨테이너를 다른 컨테이너로 변경합니다.
 * @code
 * using MyTuple = std::tuple<int, float>;
 * using MyVariant = sw::rebind_t<MyTuple, std::variant>; // std::variant<int, float>
 * @endcode
 */
template <typename Tuple, template <typename...> typename Target>
using rebind_t = internal::rebind_impl<Tuple>::template to<Target>;

/** 여러 튜플 타입을 하나로 합친 타입을 반환합니다. */
template <typename... Tuples>
using tuple_cat_t = internal::tuple_cat_t<Tuples...>;

/**
 * 중첩된 튜플을 평탄화(Flatten)합니다.
 * @code
 * using Nested = std::tuple<int, std::tuple<float, double>>;
 * using Flat = sw::flatten_tuple_t<Nested>; // std::tuple<int, float, double>
 * @endcode
 */
template <typename T, template <typename...> typename ResultTupleLike = std::tuple>
using flatten_tuple_t = internal::flatten_impl<ResultTupleLike, T>::type;
} // namespace sw
