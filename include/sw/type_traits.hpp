#pragma once
#include <tuple>
#include <type_traits>

#include "sw/types.hpp"


namespace sw
{
// -------------------------------------------------------------------------
// Function Traits: 함수의 반환형, 인자 타입, 인자 개수 등을 컴파일 타임에 분석
// -------------------------------------------------------------------------
template <typename, typename = void>
struct function_traits : std::false_type {};

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    using signature = R(Args...);
    using return_type = R;
    using argument_types = std::tuple<Args...>;

    /** 함수의 인자 개수 */
    static constexpr usize arity = sizeof...(Args);
};

// 함수 포인터
template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

// 멤버 함수 포인터
template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> {};

// const 멤버 함수
template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(Args...)> {};

// volatile 멤버 함수
template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) volatile> : function_traits<R(Args...)> {};

// 람다/함수 객체 지원 (operator() 사용)
template <typename Fn>
struct function_traits<Fn, std::void_t<decltype(&Fn::operator())>> : function_traits<decltype(&Fn::operator())> {};
} // namespace sw
