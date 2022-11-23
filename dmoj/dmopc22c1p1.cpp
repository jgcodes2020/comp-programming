/*
jgcodes's dead simple CP template
*/

#include <bits/stdc++.h>
#include <memory>

#define SUBTASK 2

// Rust-style ints
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using isz = size_t;

using c8 = char;

template <class T>
using vec = std::vector<T>;
template <class T, size_t N>
using arr = std::array<T, N>;

// scanf, but it will throw on error, crashing your program
// instead of causing UB
template <class... Ts>
inline void scanf2(const char* format, Ts&... args) {
  int res = scanf(format, std::addressof(args)...);
  if ((size_t) res < sizeof...(Ts))
    throw std::runtime_error("scanf didn't scan all args");
}

arr<i32, isz(1e6) + 2> a {};

void run_case() {
  isz n;

  

  // F = down, T = up
#if SUBTASK <= 1
  scanf2("%zd\n", n);
  for (isz i = 0; i < n; i++) {
    scanf2("%d", a[i]);
  }

  if (a[0] == a[1]) {
    puts("NO");
    return;
  }
  bool pd = a[0] < a[1];
  for (isz i = 1; i < n - 1; pd = !pd, i++) {
    if (a[i] == a[i + 1]) {
      puts("NO");
      return;
    }
    if ((a[i] < a[i + 1]) != pd) {
      puts("NO");
      return;
    }
  }
  
  puts("YES");
#elif SUBTASK <= 2
  scanf2("%d\n", n);
  for (isz i = 0; i < n; i++) {
    scanf2("%d", a[i]);
  }

  // multiple "directional" pairs
  // in different spots can mess with each other
  bool pd;
  bool pd_found = false;

  for (isz i = 0; i < n - 1; i++) {
    if (a[i] != 0 && a[i + 1] != 0) {
      if (a[i] == a[i + 1]) {
        puts("NO");
        return;
      }
      if (!pd_found) {
        pd_found = true;
        pd       = (a[i] < a[i + 1]) != (i % 2 == 0);
      }
      else {
        bool in_dir = (a[i] < a[i + 1]) != (i % 2 == 0);
        if (in_dir != pd) {
          puts("NO");
          return;
        }
      }
    }
  }

  puts("YES");
#endif
}

int main() {
  isz t;
  scanf2("%zd", t);
  for (isz tc = 0; tc < t; tc++) {
    run_case();
  }
}