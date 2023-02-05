#include <bits/stdc++.h>
using namespace std;

using uint = unsigned;

uint get_stat(const string& x) {
  if (x.length() >= 2 && x[x.length() - 2] == '1') {
    switch (x[x.length() - 1]) {
      case '1':
        return 2;
      case '7':
        return 3;
      default:
        return 0;
    }
  }
  else if (x.length() >= 1 && x[x.length() - 1] == '7') {
    return 1;
  }
  return 0;
}

int main() {
  uint cnt;
  cin >> cnt;
  
  string na, nb;
  uint sta, stb;
  for (uint i = 0; i < cnt; i++) {
    cin >> na >> nb;
    sta = get_stat(na);
    stb = get_stat(nb);
    if ((sta == 1 && stb == 2) || (sta == 2 && stb == 1)) {
      cout << "YES\n";
    }
    else {
      cout << "NO\n";
    }
  }
}
