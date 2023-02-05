// Will leave all solutions I find here
#define SUBTASK 4

#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#if SUBTASK <= 2
#pragma region BEGIN SOLUTION
  #include <cstddef>
  #include <cstdint>
  #include <cstdio>
  #include <cstdlib>
  #include <vector>

// Like scanf, but won't cause uninitialized
// variable issues.
template <class... Ts>
void safe_scanf(const char* fmt, Ts... values) {
  int res = scanf(fmt, values...);
  if (res < sizeof...(Ts))
    exit(69);
}

int main() {
  uint64_t n, m, k;
  safe_scanf("%ld %ld %ld", &n, &m, &k);

  // There are at least N good samples
  if (k < n) {
    puts("-1");
    return 0;
  }
  // Most possible good samples
  // N of length 1
  // N - 1 of length 2
  // ...
  // N - M + 1 of length M
  if (k > (2 * n - 1)) {
    puts("-1");
    return 0;
  }

  uint64_t sw = k - n;
  uint64_t cu = 1;
  std::vector<uint64_t> out;
  out.reserve(n);
  for (uint64_t i = 0; i < n; i++) {
    out.push_back(cu);

    if (sw > 0) {
      sw--;
      // changes 1 to 2 and vice versa
      cu ^= 3;
    }
  }

  // output the list
  {
    auto it = out.begin();
    printf("%ld", *it++);
    while (it < out.end()) {
      printf(" %ld", *it++);
    }
    puts("");
  }
}
#pragma endregion
#elif SUBTASK <= 4
#pragma region BEGIN SOLUTION
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

// Like scanf, but won't cause uninitialized
// variable issues.
template <class... Ts>
void safe_scanf(const char* fmt, Ts... values) {
  int res = scanf(fmt, values...);
  if (res == EOF)
    exit(70);
  if ((size_t) res < sizeof...(Ts))
    exit(69);
}
void print_list(const std::vector<uint64_t>& l) {
  auto it = l.begin();
  if (it < l.end())
    printf("%ld", *it++);
  while (it < l.end())
    printf(" %ld", *it++);
  puts("");
}

int main() {
  uint64_t n, m, k;
  safe_scanf("%ld %ld %ld", &n, &m, &k);
  // output array
  std::vector<uint64_t> out;
  out.reserve(n);

  // There are at least N good samples
  if (k < n) {
    puts("-1");
    return 0;
  }

  // Calculate the highest *needed* note
  // Pattern is N + (N - 1) + ... + (N - M + 2) + (N - M + 1)
  uint64_t mr;
  {
    long double b = 2*n + 1;
    long double dc = sqrtl(b*b - 8*k);
    mr = (uint64_t) ceill((b - dc) / 2.0l);
  }
  
  // Highest note is too high
  if (mr > m) {
    puts("-1");
    return 0;
  }
  
  // wind-up sequence
  for (uint64_t i = 1; i <= mr; i++) {
    out.push_back(i);
  }
  // remaining after wind-up (all rows except last)
  uint64_t rwu = n - mr;
  // remaining after wind-up for last row
  uint64_t rwul;
  {
    uint64_t mrp = mr - 1;
    uint64_t sum = mrp * (2 * n - mrp + 1) / 2;
    rwul = k - sum - 1;
    rwu -= rwul;
  }
  
  for (uint64_t i = 0; i < rwul; i++) {
    out.push_back(*(out.end() - mr));
  }
  for (uint64_t i = 0; i < rwu; i++) {
    out.push_back(*(out.end() - mr + 1));
  }
  print_list(out);
}
#pragma endregion
#endif