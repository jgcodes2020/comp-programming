/*
Davis Liu:
Given an array A of length N, how many contiguous subarrays have a sum of x?
*/

#include <bits/stdc++.h>
using namespace std;
int main() {
  int n, x;
  std::vector<int> arr, psa;
  
  cin >> n;
  arr.reserve(n);
  psa.reserve(n + 1);
  for (int i = 0; i < n; i++) {
    cin >> x;
    arr.push_back(x);
  }
  cin >> x;
  
  for (int i = 0; i < n; i++) {
    if (psa.size() > 0)
      psa.push_back(psa.back() + arr[i]);
    else
      psa.push_back(arr[i]);
    cout << psa.back() << ' ';
  }
  cout << endl;
  
  int c = 0;
  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      int s;
      if (i == 0)
        s = arr[j];
      else
        s = arr[j] - arr[i - 1];
        
      cout << "i, j, s = " << i << ", " << j << ", " << s << endl;
    }
  }
  
  cout << c << endl;
}

// 1 2 3 4
// 1 3 6 10
//         