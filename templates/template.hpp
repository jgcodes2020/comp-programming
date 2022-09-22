/*
jgcodes's dead simple CP template
*/

#include <bits/stdc++.h>

// Rust-style ints
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using c8 = char;

template <class T>
using vec = std::vector<T>;
template <class T, size_t N>
using arr = std::array<T, N>;

// scanf, but it will throw on error, crashing your program
// instead of causing UB
template <class... Ts>
inline void scanf2(const char* format, Ts&&... args) {
  int res = scanf(format, std::forward<Ts>(args)...);
  if (res == EOF)
    throw std::runtime_error("scanf hit EOF");
  if ((size_t) res < sizeof...(Ts))
    throw std::runtime_error("scanf didn't scan all args");
}