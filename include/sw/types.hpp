#pragma once

#include <cstdint>
#include <cstddef>


namespace sw
{
using i8  = std::int8_t;   // 8-bit signed
using i16 = std::int16_t;  // 16-bit signed
using i32 = std::int32_t;  // 32-bit signed
using i64 = std::int64_t;  // 64-bit signed

using u8  = std::uint8_t;  // 8-bit unsigned
using u16 = std::uint16_t; // 16-bit unsigned
using u32 = std::uint32_t; // 32-bit unsigned
using u64 = std::uint64_t; // 64-bit unsigned

using f32 = float;         // 32-bit float (Single precision)
using f64 = double;        // 64-bit float (Double precision)

using usize = std::uintptr_t;
using isize = std::intptr_t;
}

#ifndef SW_NO_GLOBAL_TYPES
using i8  = sw::i8;
using i16 = sw::i16;
using i32 = sw::i32;
using i64 = sw::i64;

using u8  = sw::u8;
using u16 = sw::u16;
using u32 = sw::u32;
using u64 = sw::u64;

using f32 = sw::f32;
using f64 = sw::f64;

using usize = sw::usize;
using isize = sw::isize;
#endif
