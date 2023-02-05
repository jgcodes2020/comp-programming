#include <bits/stdc++.h>
#include <functional>
#include <vector>
using namespace std;

int main() {
  uint n, t;
  std::vector<uint> lt, lm, ls;
  
  cin >> n >> t;
  for (uint i = 0; i < n; i++) {
    uint x;
    cin >> x;
    lt.push_back(x);
  }
  
  // prefix max/sum array
  lm.push_back(0);
  ls.push_back(lt[0]);
  for (uint i = 1; i < n; i++) {
    if (lt[i] > lt[lm[i - 1]])
      lm.push_back(i);
    else
      lm.push_back(lm[i - 1]);
    ls.push_back(ls[i - 1] + lt[i]);
  }
  
  // subtract each max
  for (uint i = 0; i < n; i++) {
    ls[i] -= lt[lm[i]];
  }
  
  // check where to stop
  uint stp_i = 0;
  for (uint i = 1; i < n; i++) {
    if (ls[i] > ls[stp_i] && ls[i] <= t)
      stp_i = i;
  }
  
  cout << (stp_i) << '\n';
}