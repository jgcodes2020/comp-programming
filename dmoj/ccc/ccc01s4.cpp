/*
jgcodes's dead simple CP template
*/

#include <bits/stdc++.h>
#include <queue>

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
template <class K, class T>
using map = std::unordered_map<K, T>;
template <class K, class T>
using smap = std::map<K, T>;
template <class T>
using set = std::unordered_set<T>;
template <class T>
using sset = std::set<T>;

template <class T>
using stk = std::stack<T, std::vector<T>>;
template <class T>
using que = std::queue<T>;
template <class T>
using pque = std::priority_queue<T>;

template <class T, class U>
using pr = std::pair<T, U>;

// scanf, but it will throw on error, crashing your program
// instead of causing UB
template <class... Ts>
inline void scanf2(const char* format, Ts&... args) {
  int res = scanf(format, std::addressof(args)...);
  if ((size_t) res < sizeof...(Ts))
    throw std::runtime_error("scanf didn't scan all args");
}

struct point {
  double x, y;
};

inline point operator+(point a, point b) {
  return point {a.x + b.x, a.y + b.y};
}

inline point operator-(point a, point b) {
  return point {a.x - b.x, a.y - b.y};
}

inline point operator*(point a, double b) {
  return point{a.x * b, a.y * b};
}
inline point operator*(double a, point b) {
  return b * a;
}

inline point operator/(point a, double b) {
  return point {a.x / b, a.y / b};
}

inline double slope(point a) {
  return a.y / a.x;
}

point circumcenter(point a, point b, point c) {
  point mid1 = (a + b) / 2, mid2 = (b + c) / 2;
  double m1 = -1 / slope(a - b), m2 = -1 / slope(b - c);
}

int main() {
  
}