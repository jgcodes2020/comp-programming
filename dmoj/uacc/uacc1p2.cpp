#include <iostream>
using namespace std;
/*
1 2 3 4 5 6 7 8 9 A B C
S T O O O T O O O T O O
O O O T O O O T O O O T

at (n % 4) == 2; tree blocks, can't use it
1 tree added every 2 tiles
*/

int lut1[] = {0, 1, 2, 5};

int main() {
  int n;
  cin >> n;
  
  int q = n / 4;
  int r = n % 4;
  
  int res = q * 6 + lut1[r] - 1;
  cout << res << '\n';
}