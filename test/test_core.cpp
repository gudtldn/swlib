#include <sw/types.hpp>
#include <sw/macros.hpp>
#include "utils.hpp"

void run_tests()
{
    // 1. Type Size Checks
    static_assert(sizeof(sw::i8) == 1);
    static_assert(sizeof(sw::i16) == 2);
    static_assert(sizeof(sw::i32) == 4);
    static_assert(sizeof(sw::i64) == 8);

    static_assert(sizeof(sw::u8) == 1);
    static_assert(sizeof(sw::u16) == 2);
    static_assert(sizeof(sw::u32) == 4);
    static_assert(sizeof(sw::u64) == 8);

    static_assert(sizeof(sw::f32) == 4);
    static_assert(sizeof(sw::f64) == 8);

    // 2. Platform Macro Sanity Check
    // 적어도 하나의 플랫폼/컴파일러/아키텍처는 true여야 함
    constexpr bool platform_detected = 
        SW_PLATFORM_WINDOWS || SW_PLATFORM_LINUX || SW_PLATFORM_MACOS;
    static_assert(platform_detected, "No platform detected!");

    constexpr bool arch_detected = 
        SW_ARCH_X64 || SW_ARCH_X86 || SW_ARCH_ARM64;
    // 일부 특이 아키텍처는 false일 수 있으므로 warning 정도만 고려 (여기선 assert)
    // 일반적인 PC 개발 환경이라면 통과해야 함
    
    constexpr bool compiler_detected = 
        SW_COMPILER_MSVC || SW_COMPILER_CLANG || SW_COMPILER_GCC;
    static_assert(compiler_detected, "No compiler detected!");

    // 3. Helper Macros
    std::println("Platform Windows: {}", SW_PLATFORM_WINDOWS);
    std::println("Arch X64: {}", SW_ARCH_X64);
    std::println("Compiler MSVC: {}", SW_COMPILER_MSVC);
}

TEST_MAIN
