#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <bits/utility.h>
#include <cstddef>
#include <type_traits>
#include <utility>

template <class T, size_t N>
class vec {
  static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
  T data[N];
  
  T& x() {
    static_assert(N >= 1, "No X component");
    return data[0];
  }
  
  T& y() {
    static_assert(N >= 2, "No Y component");
    return data[1];
  }
  
  T& z() {
    static_assert(N >= 3, "No Z component");
    return data[2];
  }
};


class vec2 {
  double x, y, z;
};

namespace details {
  template <class T, size_t N, size_t... Is>
  vec<T, N> plus_impl(const vec<T, N>& a, const vec<T, N>& b, std::index_sequence<Is...>) {
    return vec<T, N> {a.data[Is] + b.data[Is]...};
  }
}

template <class T, size_t N>
vec<T, N> operator+(const vec<T, N>& a, const vec<T, N>& b) {
  return details::plus_impl(a, b, std::make_index_sequence<N> {});
}

#endif