#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <iterator>
#include <set>
#include <vector>

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

using std::cin, std::cout;

inline bool is_pow3(i64 x) {
  constexpr i64 max_pow = 3486784401;
  return max_pow % x == 0;
}

inline std::vector<i64> cantor_table(i64 n) {
  std::vector<i64> values {0, 1, 2, 3};
  int two_thirds = 6;
  for (int i = 1; i < n; i++) {
    size_t cs = values.size();
    for (size_t i = 0; i < cs; i++) {
      values.push_back(values[i] + two_thirds);
    }
    two_thirds *= 3;
  }
  return values;
}

int main() {
  i64 d;
  cin >> d;

  if (is_pow3(d)) {
    // log_3 x = log x / log 3
    auto values = cantor_table(std::log2(d) / std::log2(3));
    for (i64 n : values) {
      cout << n << '\n';
    }
  }
  else {
    i64 h_exp = 19;
    i64 pow_3 = std::pow(3, h_exp);
    auto table = cantor_table(h_exp);
    for (int n = 0; n < d; n++) {
      auto cvt = (n * pow_3 / d);
      auto bound = std::upper_bound(table.begin(), table.end(), cvt);
      // cout << n << "/" << d << " -> " << cvt << "/" << pow_3 << " -> " << *bound << "/" << pow_3 << '\n';
      if (bound != table.begin() && *(bound - 1) == cvt) {
        cout << n << '\n';

      }
      else if ((bound - table.begin()) % 2 == 1) {
        cout << n << '\n';
      }
    }
    cout << d << '\n';
  }
}

// 3^1: 0, 1 | 2, 3
// 3^2: 0, 1 | 2, 3 | 6, 7 | 8, 9
// 3^3: 0, 1 | 2, 3 | 6, 7 | 8, 9 | 18, 19 | 20, 21 | 24, 25 | 26, 27

// n/d = x/(3^p)
// x = n * (3^p) / d