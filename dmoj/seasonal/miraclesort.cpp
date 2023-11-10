#include <iostream>
#include <cstdint>

using namespace std;

int main() {
  int count;
  int64_t x;
  
  cin >> count;
  for (int i = 0; i < count; i++) {
    cin >> x;
    cout << x << '\n';
  }
}