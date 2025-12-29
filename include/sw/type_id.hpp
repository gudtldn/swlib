#pragma once

#include <string_view>
#include <compare>
#include <functional>

#include "sw/hash.hpp"
#include "sw/type_signature.hpp"


namespace sw
{
/**
 * 컴파일 타임 타입 식별자 (Type Identifier)
 */
class type_id
{
public:
    constexpr type_id() = default;

    /** 템플릿 타입 T에 대한 type_id를 반환합니다. */
    template <typename T>
    [[nodiscard]] static constexpr type_id get() noexcept
    {
        if constexpr (std::is_function_v<std::remove_pointer_t<std::decay_t<T>>>)
        {
            constexpr auto signature = type_signature<T>();
            return type_id{ signature, signature };
        }
        else
        {
            using clean_type = std::remove_cvref_t<T>;
            return type_id{ full_type_name<clean_type>(), type_signature<clean_type>() };
        }
    }

public:
    /** 타입 이름을 반환합니다. */
    [[nodiscard]] constexpr std::string_view name() const noexcept { return type_name; }

    /** 타입 해시를 반환합니다. */
    [[nodiscard]] constexpr u64 hash() const noexcept { return type_hash; }

    /** TypeId가 올바른지 확인합니다. */
    [[nodiscard]] constexpr bool is_valid() const noexcept { return type_hash != 0; }

public:
    [[nodiscard]] explicit constexpr operator bool() const { return is_valid(); }
    [[nodiscard]] constexpr bool operator==(const type_id& other) const { return type_hash == other.type_hash; }
    [[nodiscard]] constexpr auto operator<=>(const type_id& other) const { return type_hash <=> other.type_hash; }

private:
    constexpr type_id(std::string_view name, std::string_view hash_sig)
        : type_name(name)
        , type_hash(fnv1a(hash_sig))
    {
    }

    std::string_view type_name;
    u64 type_hash = 0;
};
} // namespace sw

template <>
struct std::hash<sw::type_id>
{
    constexpr std::size_t operator()(const sw::type_id& id) const noexcept
    {
        return static_cast<std::size_t>(id.hash());
    }
};
