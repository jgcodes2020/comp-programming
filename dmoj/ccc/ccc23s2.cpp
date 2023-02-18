#include <bits/stdc++.h>
using std::cin, std::cout, std::string, std::getline, std::endl;

int dp[5002][5002];
int mins[5002];
int l[5002];

static inline int absdiff(int x, int y) {
  return (x > y) ? x - y : y - x;
}

int main() {
  int n, tmp1;
  cin >> n;
  
  for (int i = 0; i < n; i++) {
    cin >> l[i];
  }
  
  for (int i = 0; i < n; i++) {
    dp[i][0] = 0;
    dp[i][1] = 0;
  }
  for (int i = 2; i <= n; i++) {
    mins[i] = -1;
  }
  mins[1] = 0;
  
  
  for (int j = 2; j <= n; j++) {
    for (int i = 0; i <= n - j; i++) {
      //
      dp[i][j] = dp[i + 1][j - 2] + absdiff(l[i], l[i + j - 1]);
      if (mins[j] == -1 || dp[i][j] < mins[j])
        mins[j] = dp[i][j];
      //cout << std::setw(5) << dp[i][j] << ' ';
    }
    
    //cout << endl;
  }
  
  for (int i = 1; i <= n; i++) {
    cout << mins[i];
    cout << ' ';
  }
  cout << endl;
}