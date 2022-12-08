/*
jgcodes's dead simple CP template
*/

#include <bits/stdc++.h>
#include <vector>
using namespace std;

// scanf, but it will throw on error, crashing your program
// instead of causing UB
template <class... Ts>
inline void scanf2(const char* format, Ts&... args) {
  int res = scanf(format, std::addressof(args)...);
  if ((size_t) res < sizeof...(Ts))
    throw std::runtime_error("scanf didn't scan all args");
}

int true_mod(int a, int b) {
  int mod = (a % b);
  return (-int(mod < 0) & b) + mod;
}

std::array<int, size_t(1e6)> inp {};
std::array<int, size_t(1e6)+1> psa {};

int main() {
  int n, c;
  scanf2("%d %d\n", n, c);
  
  for (int i = 0; i < n; i++) {
    int in;
    scanf2("%d ", in);
    inp[in]++;
  }
  
  // setup PSA
  psa[0] = 0;
  for (int i = 1; i <= c; i++) {
    psa[i] = psa[i - 1] + inp[i - 1];
  }
  
  for (int i = 0;)
}