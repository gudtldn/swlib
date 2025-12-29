// ReSharper disable CppDFALocalValueEscapesFunction
#pragma once

#include <array>
#include <string_view>
#include <type_traits>

#include "sw/types.hpp"


namespace sw
{
namespace details
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
consteval std::string_view remove_tokens(std::string_view signature, const std::array<std::string_view, N>& tokens) noexcept
{
    bool modified;
    do
    {
        modified = false;
        for (const auto& token : tokens)
        {
            if (signature.starts_with(token))
            {
                // 토큰 뒤가 경계 문자(공백 등)인지 확인하여 오탐 방지 (예: class_name vs class)
                const char next = (signature.size() > token.size()) ? signature[token.size()] : '\0';
                if (is_token_boundary(next))
                {
                    signature.remove_prefix(token.size());
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
struct extract_base_type { using type = T; };

template <typename T>
struct extract_base_type<T*> : extract_base_type<std::remove_cv_t<T>> {};

template <typename T>
struct extract_base_type<T&> : extract_base_type<std::remove_cv_t<T>> {};

template <typename T>
struct extract_base_type<T&&> : extract_base_type<std::remove_cv_t<T>> {};

template <typename T>
struct extract_base_type<T[]> : extract_base_type<std::remove_cv_t<T>> {};

template <typename T, usize N>
struct extract_base_type<T[N]> : extract_base_type<std::remove_cv_t<T>> {};

template <typename T>
using extract_base_type_t = extract_base_type<std::remove_cvref_t<T>>::type;

/**
 * 현재 컴파일러에서 사용 가능한 타입 시그니처 정보를 raw 형태로 반환합니다.
 * @return 컴파일러에 종속적인 타입 시그니처 문자열을 반환합니다.
 */
template <typename T>
consteval std::string_view get_raw_signature() noexcept
{
#ifdef _MSC_VER
    return __FUNCSIG__;
#elif defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#else
#error "Unsupported compiler for type name extraction"
#endif
}

/** MSVC 타입 시그니처에서 원본 타입명을 추출합니다. */
consteval std::string_view extract_type_msvc(std::string_view in_signature) noexcept
{
    constexpr std::string_view prefix = "get_raw_signature<";
    usize start_pos = in_signature.find(prefix);
    if (start_pos == std::string_view::npos)
    {
        return {};
    }
    start_pos += prefix.size();

    constexpr std::string_view suffix = ">(void) noexcept";
    const usize end_pos = in_signature.rfind(suffix);
    if (end_pos == std::string_view::npos || end_pos <= start_pos)
    {
        return {};
    }

    // <>안 Type 정보만 추출
    const std::string_view extracted_typename = trim_whitespace(in_signature.substr(start_pos, end_pos - start_pos));
    return extracted_typename;
}

/** GCC/Clang 타입 시그니처에서 원본 타입명을 추출합니다. */
consteval std::string_view extract_type_gcc_clang(std::string_view in_signature) noexcept
{
#if defined(__clang__)
    constexpr std::string_view prefix = "[T = ";
#else
    constexpr std::string_view prefix = "[with T = ";
#endif
    usize start_pos = in_signature.find(prefix);
    if (start_pos == std::string_view::npos)
    {
        return {};
    }
    start_pos += prefix.size();


#if defined(__clang__)
    constexpr std::string_view suffix = "]";
#else
    constexpr std::string_view suffix = ";";
#endif
    const usize end_pos = in_signature.rfind(suffix);
    if (end_pos == std::string_view::npos || end_pos <= start_pos)
    {
        return {};
    }

    // [with T = ;]안 Type 정보만 추출
    const std::string_view extracted_typename = in_signature.substr(start_pos, end_pos - start_pos);
    return extracted_typename;
}

/** 컴파일 타임에 템플릿 타입 T에서 원본 타입명을 추출합니다. */
template <typename T>
consteval std::string_view extract_type_name() noexcept
{
    constexpr auto signature = get_raw_signature<T>();

#ifdef _MSC_VER
    return extract_type_msvc(signature);
#elif defined(__clang__) || defined(__GNUC__)
    return extract_type_gcc_clang(signature);
#else
    return {};
#endif
}
} // namespace details

/**
 * 컴파일러가 제공하는 원본 타입 시그니처를 반환합니다.
 * @tparam T 시그니처를 추출할 대상 타입
 * @return 컴파일러가 생성한 원본 타입 시그니처
 */
template <typename T>
[[nodiscard]] consteval std::string_view type_signature() noexcept
{
    constexpr auto ret = details::extract_type_name<T>();

    // IDE 버그 때문에 일단 주석
    // static_assert(!ret.empty(), "Failed to extract type name from type T");

    return ret;
}

/**
 * 타입의 namespace를 포함한 순수 타입 이름을 컴파일 타임에 추출합니다.
 *
 * @tparam T 타입 이름을 추출할 대상 타입
 * @return namespace를 포함한 순수 타입 이름
 */
template <typename T>
[[nodiscard]] consteval std::string_view full_type_name() noexcept
{
    using CleanType = details::extract_base_type_t<T>;
    constexpr auto signature = details::extract_type_name<CleanType>();

    // 선행 타입 키워드 ("class", "struct", "enum", "union") 제거
    constexpr std::array<std::string_view, 5> leading_keywords = { "class", "struct", "enum", "union", "typename" };
    constexpr auto ret = details::remove_tokens(signature, leading_keywords);

    // IDE 버그 때문에 일단 주석
    // static_assert(!ret.empty(), "Failed to extract type name from type T");

    return ret;
}

/**
 * 타입의 네임스페이스를 제외한 순수 타입 이름을 컴파일 타임에 추출합니다.
 *
 * @tparam T 타입 이름을 추출할 대상 타입
 * @return 네임스페이스가 제거된 순수 타입 이름
 */
template <typename T>
[[nodiscard]] consteval std::string_view type_name() noexcept
{
    constexpr auto full_name = full_type_name<T>();
    return details::remove_namespace(full_name);
}
} // namespace sw
