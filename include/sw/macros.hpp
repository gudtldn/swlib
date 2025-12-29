#pragma once

// -----------------------------------------------------------------------------
// Platform Detection
// -----------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
    #define SW_PLATFORM_WINDOWS true
#else
    #define SW_PLATFORM_WINDOWS false
#endif

#if defined(__linux__)
    #define SW_PLATFORM_LINUX true
#else
    #define SW_PLATFORM_LINUX false
#endif

#if defined(__APPLE__)
    #define SW_PLATFORM_MACOS true
#else
    #define SW_PLATFORM_MACOS false
#endif

// -----------------------------------------------------------------------------
// Architecture Detection
// -----------------------------------------------------------------------------
#if defined(_M_X64) || defined(__x86_64__)
    #define SW_ARCH_X64 true
    #define SW_ARCH_X86 false
    #define SW_ARCH_ARM64 false
#elif defined(_M_IX86) || defined(__i386__)
    #define SW_ARCH_X64 false
    #define SW_ARCH_X86 true
    #define SW_ARCH_ARM64 false
#elif defined(_M_ARM64) || defined(__aarch64__)
    #define SW_ARCH_X64 false
    #define SW_ARCH_X86 false
    #define SW_ARCH_ARM64 true
#else
    #define SW_ARCH_X64 false
    #define SW_ARCH_X86 false
    #define SW_ARCH_ARM64 false
#endif

// -----------------------------------------------------------------------------
// Compiler Detection
// -----------------------------------------------------------------------------
#if defined(_MSC_VER)
    #define SW_COMPILER_MSVC true
    #define SW_COMPILER_CLANG false
    #define SW_COMPILER_GCC false
#elif defined(__clang__)
    #define SW_COMPILER_MSVC false
    #define SW_COMPILER_CLANG true
    #define SW_COMPILER_GCC false
#elif defined(__GNUC__)
    #define SW_COMPILER_MSVC false
    #define SW_COMPILER_CLANG false
    #define SW_COMPILER_GCC true
#else
    #define SW_COMPILER_MSVC false
    #define SW_COMPILER_CLANG false
    #define SW_COMPILER_GCC false
#endif

// -----------------------------------------------------------------------------
// Build Configuration
// -----------------------------------------------------------------------------
#if defined(_DEBUG) || defined(DEBUG)
    #define SW_BUILD_DEBUG true
    #define SW_BUILD_RELEASE false
#else
    #define SW_BUILD_DEBUG false
    #define SW_BUILD_RELEASE true
#endif

// -----------------------------------------------------------------------------
// Utility Macros (Inlining, DebugBreak, etc.)
// -----------------------------------------------------------------------------

// Force Inline
#if SW_COMPILER_MSVC
    #define SW_FORCE_INLINE __forceinline
    #define SW_NO_INLINE __declspec(noinline)
#elif SW_COMPILER_CLANG || SW_COMPILER_GCC
    #define SW_FORCE_INLINE __attribute__((always_inline)) inline
    #define SW_NO_INLINE __attribute__((noinline))
#else
    #define SW_FORCE_INLINE inline
    #define SW_NO_INLINE
#endif

// Debug Break
#if SW_COMPILER_MSVC
    #define SW_DEBUGBREAK() __debugbreak()
#elif SW_COMPILER_CLANG || SW_COMPILER_GCC
    #define SW_DEBUGBREAK() __builtin_trap()
#else
    #define SW_DEBUGBREAK()
#endif

#define SW_CONCAT_IMPL(a, b) a##b
#define SW_CONCAT(a, b) SW_CONCAT_IMPL(a, b)

#define SW_UNIQUE_NAME(prefix) SW_CONCAT(prefix, __COUNTER__)

#define SW_STRINGIFY_IMPL(x) #x
#define SW_STRINGIFY(x) SW_STRINGIFY_IMPL(x)
