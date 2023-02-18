#include <bits/stdc++.h>
using std::cin, std::cout, std::string, std::getline, std::endl;

// if y is true, is equal to x, if false, it is zero
inline int bten(int x, bool y) {
  return y ? x : 0;
}

int main() {
  int n, tmp1;
  cin >> n;

  std::vector<bool> l1, l2;
  for (int i = 0; i < n; i++) {
    cin >> tmp1;
    l1.push_back(tmp1 != 0);
  }
  for (int i = 0; i < n; i++) {
    cin >> tmp1;
    l2.push_back(tmp1 != 0);
  }

  int c = bten(3, l1[0]) + bten(3, l2[0]) - bten(2, l1[0] && l2[0]);
  bool even;

  for (int i = 1; i < n; i++) {
    even = (i % 2 == 0);
    // add new edges
    c += bten((l1[i - 1]? 1 : 3), l1[i]);
    c += bten((l2[i - 1]? 1 : 3), l2[i]);
    // middle edge
    c -= bten(2, l1[i] && l2[i] && even);
  }
  cout << c << endl;
}
/*
E O
0 1
1 0
*/