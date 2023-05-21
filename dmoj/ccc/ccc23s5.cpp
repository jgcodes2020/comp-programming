#include <cstdio>
#include <set>
#include <vector>

int main() {
  int d;
  int p, q;
  bool flag;
  std::set<int> past_rems;
  
  scanf("%d", &d);
  
  for (int i = 0; i <= d; i++) {
    past_rems.clear();
    p = i;
    past_rems.emplace(p);
    // compute ternary expansion of p/q
    flag = true;
    while (true) {
      p = (3 * p);
      q = p / d;
      p = p % d;
      if (q == 1 && p != 0) {
        flag = false;
        break;
      }
      if (p == 0 || past_rems.count(p))
        break;
      past_rems.emplace(p);
    }
    if (flag) {
      printf("%d\n", i);
    }
  }
  
}