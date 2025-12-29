#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "sw/types.hpp"


namespace sw
{
namespace details
{
// SBO(Small Buffer Optimization) 설정
constexpr usize sbo_buffer_size = sizeof(void*) * 3;
using sbo_align = std::max_align_t;
}

template <typename Signature>
class function;

template <typename ReturnType, typename... ParamsType>
class function<ReturnType(ParamsType...)>
{
private:
    /** Type Erasure Interface */
    struct callable_base
    {
        virtual ~callable_base() = default;
        virtual ReturnType invoke(ParamsType&&...) = 0;

        virtual callable_base* clone_to_heap() const = 0;
        virtual callable_base* clone_to_buffer(void* dest) const = 0;
        virtual callable_base* move_to_buffer(void* dest) noexcept = 0;
    };

    /** Concrete Implementation */
    template <typename Fn>
    struct callable_impl final : callable_base
    {
        Fn functor;

        template <typename F>
            requires (!std::same_as<std::decay_t<F>, callable_impl>)
        explicit callable_impl(F&& func)
            : functor(std::forward<F>(func))
        {
        }

        virtual ReturnType invoke(ParamsType&&... args) override
        {
            return std::invoke(functor, std::forward<ParamsType>(args)...);
        }

        virtual callable_base* clone_to_heap() const override
        {
            return new callable_impl(functor);
        }

        virtual callable_base* clone_to_buffer(void* dest) const override
        {
            return std::construct_at(static_cast<callable_impl*>(dest), functor);
        }

        virtual callable_base* move_to_buffer(void* dest) noexcept override
        {
            return std::construct_at(static_cast<callable_impl*>(dest), std::move(functor));
        }
    };

public:
    function() noexcept = default;

    function(std::nullptr_t) noexcept
    {
    }

    ~function()
    {
        destroy();
    }

    function(const function& other)
    {
        if (other.callable)
        {
            if (other.is_on_heap())
            {
                // 힙 -> 힙 복사
                callable = other.callable->clone_to_heap();
                storage.heap_storage = callable;
            }
            else
            {
                // SBO -> SBO 복사
                callable = other.callable->clone_to_buffer(storage.sbo_storage);
            }
        }
    }

    function(function&& other) noexcept
    {
        if (other.callable)
        {
            if (other.is_on_heap())
            {
                // 힙 포인터만 이동
                storage.heap_storage = other.storage.heap_storage;
                callable = other.callable;
            }
            else
            {
                // SBO 데이터 이동
                callable = other.callable->move_to_buffer(storage.sbo_storage);
            }

            // 원본 초기화 (소멸자에서 해제 안되도록)
            other.storage.heap_storage = nullptr;
            other.callable = nullptr;
        }
    }

    function& operator=(const function& other)
    {
        if (this != &other)
        {
            function temp(other);
            swap(temp);
        }
        return *this;
    }

    function& operator=(function&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            if (other.callable)
            {
                if (other.is_on_heap())
                {
                    // 힙 포인터만 이동
                    storage.heap_storage = other.storage.heap_storage;
                    callable = other.callable;
                }
                else
                {
                    // SBO 데이터 이동
                    callable = other.callable->move_to_buffer(storage.sbo_storage);
                }

                // 원본 초기화 (소멸자에서 해제 안되도록)
                other.storage.heap_storage = nullptr;
                other.callable = nullptr;
            }
        }
        return *this;
    }

    function& operator=(std::nullptr_t) noexcept
    {
        destroy();
        return *this;
    }

    template <typename Fn>
        requires (
            !std::same_as<std::decay_t<Fn>, function>                // 자기 자신은 제외
            && !std::same_as<std::decay_t<Fn>, std::nullptr_t>       // nullptr_t는 별도 생성자에서 처리
            && std::is_invocable_r_v<ReturnType, Fn&, ParamsType...> // 호출 가능성 검사 (반환 타입 포함)
        )
    function(Fn&& func)
    {
        using decayed_type = std::decay_t<Fn>;
        using impl_type = callable_impl<decayed_type>;

        // SBO 조건 검사: 크기가 버퍼보다 작고, 이동 생성이 noexcept여야 함
        constexpr bool use_sbo = (sizeof(impl_type) <= details::sbo_buffer_size)
            && std::is_nothrow_move_constructible_v<decayed_type>;

        if constexpr (use_sbo)
        {
            callable = std::construct_at(reinterpret_cast<impl_type*>(storage.sbo_storage), std::forward<Fn>(func));
        }
        else
        {
            callable = new impl_type(std::forward<Fn>(func));
            storage.heap_storage = callable;
        }
    }

public:
    void swap(function& other) noexcept
    {
        std::swap(*this, other);
    }

    [[nodiscard]] bool is_valid() const noexcept
    {
        return callable != nullptr;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return is_valid();
    }

    ReturnType operator()(ParamsType... args) const
    {
        if (!callable) [[unlikely]]
        {
            throw std::bad_function_call();
        }
        return callable->invoke(std::forward<ParamsType>(args)...);
    }

    [[nodiscard]] bool operator==(std::nullptr_t) const noexcept
    {
        return !is_valid();
    }

private:
    [[nodiscard]] bool is_on_heap() const noexcept
    {
        // callable_이 sbo_buf의 주소와 다르면 힙에 있는 것임
        return callable != reinterpret_cast<const callable_base*>(storage.sbo_storage);
    }

    void destroy() noexcept
    {
        if (callable)
        {
            if (is_on_heap())
            {
                delete callable;
            }
            else
            {
                std::destroy_at(callable);
            }
            callable = nullptr;
        }
    }

private:
    union func_storage_t
    {
        callable_base* heap_storage;
        alignas(details::sbo_align) u8 sbo_storage[details::sbo_buffer_size];
    } storage;

    callable_base* callable = nullptr;
};
} // namespace sw
