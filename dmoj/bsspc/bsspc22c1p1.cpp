#include <bits/stdc++.h>
using namespace std;

using uint = unsigned;

int main() {
  uint len;
  cin >> len;
  
  uint acc = 0;
  uint curr;
  for (int i = 0; i < len; i++) {
    cin >> curr;
    acc += curr;
    cout << acc << '\n';
  }
}