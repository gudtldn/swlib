// ReSharper disable CppDFALocalValueEscapesFunction
#pragma once

#include <array>
#include <string_view>
#include <type_traits>

#include "sw/types.hpp"
#include "sw/macros.hpp"


namespace sw
{
namespace internal
{
/** 문자열 앞뒤 공백 제거 */
[[nodiscard]] consteval std::string_view trim_whitespace(std::string_view sv) noexcept
{
    constexpr std::string_view whitespace = " \t\n\r\f\v";
    const auto first = sv.find_first_not_of(whitespace);
    if (first == std::string_view::npos)
    {
        return {};
    }
    const auto last = sv.find_last_not_of(whitespace);
    return sv.substr(first, last - first + 1);
}

/** 토큰 경계 문자 확인 */
[[nodiscard]] consteval bool is_token_boundary(char c) noexcept
{
    // C++ 타입 선언에서 토큰 경계로 취급할 수 있는 문자 집합
    return c == ' ' || c == '*'
        || c == '&' || c == ','
        || c == '<' || c == '>'
        || c == ':' || c == '\0';
}

/** 특정 접두사(키워드) 제거 (struct, class, enum 등) */
template <usize N>
consteval std::string_view remove_keywords(std::string_view signature, const std::array<std::string_view, N>& keywords) noexcept
{
    bool modified;
    do
    {
        modified = false;
        for (const auto& keyword : keywords)
        {
            if (signature.starts_with(keyword))
            {
                // 키워드 뒤가 경계 문자(공백 등)인지 확인하여 오탐 방지 (예: class_name vs class)
                const char next = (signature.size() > keyword.size()) ? signature[keyword.size()] : '\0';
                if (is_token_boundary(next))
                {
                    signature.remove_prefix(keyword.size());
                    modified = true;
                    break;
                }
            }
        }

        if (modified)
        {
            signature = trim_whitespace(signature);
        }
    }
    while (modified);
    return signature;
}

/** namespace 제거 (마지막 :: 뒤의 이름만 추출) */
consteval std::string_view remove_namespace(std::string_view signature) noexcept
{
    constexpr std::string_view ns_sep = "::";
    const auto pos = signature.rfind(ns_sep);
    if (pos == std::string_view::npos)
    {
        return signature;
    }
    return signature.substr(pos + ns_sep.size());
}

/** 포인터, 참조, 배열, const/volatile을 모두 제거한 '기본 타입' 추출 */
template <typename T>
struct unwrap_type { using type = T; };

template <typename T>
struct unwrap_type<T*> : unwrap_type<std::remove_cv_t<T>> {};

template <typename T>
struct unwrap_type<T&> : unwrap_type<std::remove_cv_t<T>> {};

template <typename T>
struct unwrap_type<T&&> : unwrap_type<std::remove_cv_t<T>> {};

template <typename T>
struct unwrap_type<T[]> : unwrap_type<std::remove_cv_t<T>> {};

template <typename T, usize N>
struct unwrap_type<T[N]> : unwrap_type<std::remove_cv_t<T>> {};

template <typename T>
using unwrap_type_t = unwrap_type<std::remove_cvref_t<T>>::type;

/**
 * 현재 컴파일러에서 사용 가능한 원본 시그니처 정보를 반환합니다.
 * @return 컴파일러에 종속적인 함수 시그니처 문자열
 */
template <typename T>
consteval std::string_view get_raw_signature() noexcept
{
#if SW_COMPILER_MSVC
    return __FUNCSIG__;
#elif SW_COMPILER_CLANG || SW_COMPILER_GCC
    return __PRETTY_FUNCTION__;
#else
#error "Unsupported compiler for type name extraction"
#endif
}

/** MSVC 시그니처에서 타입 이름을 추출합니다. */
consteval std::string_view extract_name_msvc(std::string_view signature) noexcept
{
    constexpr std::string_view prefix = "get_raw_signature<";
    usize start_pos = signature.find(prefix);
    if (start_pos == std::string_view::npos)
    {
        return {};
    }
    start_pos += prefix.size();

    constexpr std::string_view suffix = ">(void) noexcept";
    const usize end_pos = signature.rfind(suffix);
    if (end_pos == std::string_view::npos || end_pos <= start_pos)
    {
        return {};
    }

    // <>안 Type 정보만 추출
    const std::string_view extracted_typename = trim_whitespace(signature.substr(start_pos, end_pos - start_pos));
    return extracted_typename;
}

/** GCC/Clang 시그니처에서 타입 이름을 추출합니다. */
consteval std::string_view extract_name_gcc_clang(std::string_view signature) noexcept
{
#if SW_COMPILER_CLANG
    constexpr std::string_view prefix = "[T = ";
#else
    constexpr std::string_view prefix = "[with T = ";
#endif
    usize start_pos = signature.find(prefix);
    if (start_pos == std::string_view::npos)
    {
        return {};
    }
    start_pos += prefix.size();

#if SW_COMPILER_CLANG
    constexpr std::string_view suffix = "]";
#else
    constexpr std::string_view suffix = ";";
#endif
    const usize end_pos = signature.rfind(suffix);
    if (end_pos == std::string_view::npos || end_pos <= start_pos)
    {
        return {};
    }

    // [with T = ;]안 Type 정보만 추출
    const std::string_view extracted_typename = signature.substr(start_pos, end_pos - start_pos);
    return extracted_typename;
}

/** 컴파일 타임에 템플릿 타입 T의 시그니처에서 이름을 추출합니다. */
template <typename T>
consteval std::string_view extract_type_name() noexcept
{
    constexpr auto signature = get_raw_signature<T>();

#if SW_COMPILER_MSVC
    return extract_name_msvc(signature);
#elif SW_COMPILER_CLANG || SW_COMPILER_GCC
    return extract_name_gcc_clang(signature);
#else
    return {};
#endif
}
} // namespace internal

/**
 * 컴파일러 시그니처에서 추출된 그대로의 타입 이름을 반환합니다. (예: "class sw::MyClass", "struct Foo")
 * @tparam T 이름을 추출할 대상 타입
 */
template <typename T>
[[nodiscard]] consteval std::string_view raw_type_name() noexcept
{
    constexpr auto ret = internal::extract_type_name<T>();

    // IDE 버그 때문에 일단 주석
    // static_assert(!ret.empty(), "Failed to extract type name from type T");

    return ret;
}

/**
 * 타입 키워드(struct, class 등)를 제거한 전체 타입 이름을 반환합니다. (예: "sw::MyClass")
 * @tparam T 타입 이름을 추출할 대상 타입
 */
template <typename T>
[[nodiscard]] consteval std::string_view full_type_name() noexcept
{
    using CleanType = internal::unwrap_type_t<T>;
    constexpr auto raw_name = internal::extract_type_name<CleanType>();

    // 선행 타입 키워드 ("class", "struct", "enum", "union") 제거
    constexpr std::array<std::string_view, 5> leading_keywords = { "class", "struct", "enum", "union", "typename" };
    constexpr auto ret = internal::remove_keywords(raw_name, leading_keywords);

    // IDE 버그 때문에 일단 주석
    // static_assert(!ret.empty(), "Failed to extract type name from type T");

    return ret;
}

/**
 * 타입에서 네임스페이스를 제외한 순수 타입 이름을 반환합니다. (예: "MyClass")
 * @tparam T 타입 이름을 추출할 대상 타입
 */
template <typename T>
[[nodiscard]] consteval std::string_view type_name() noexcept
{
    constexpr auto full_name = full_type_name<T>();
    return internal::remove_namespace(full_name);
}
} // namespace sw
