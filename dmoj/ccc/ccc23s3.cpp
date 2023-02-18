#include <bits/stdc++.h>
using std::cin, std::cout, std::string, std::getline, std::endl;

char poster[2002][2002];

int main() {
  int n, m, r, c;
  cin >> n >> m >> r >> c;
  if (r == 1 && c == 1) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        poster[i][j] = 'a' + (std::min(i, j) % 3);
      }
    }
  }
  else if (n == 2 && m == 2) {
    if (r == c) {
      switch (r) {
        case 0: cout << "ab\ncd\n" << std::flush; return 0;
        case 1: cout << "aa\nab\n" << std::flush; return 0;
        case 2: cout << "aa\naa\n" << std::flush; return 0;
      }
    }
    else if (c == 0) {
      switch (r) {
        case 1: cout << "aa\nbc\n" << std::flush; return 0;
        case 2: cout << "aa\nbb\n" << std::flush; return 0;
      }
    }
    else if (r == 0) {
      switch (c) {
        case 1: cout << "ab\nac\n" << std::flush; return 0;
        case 2: cout << "ab\nab\n" << std::flush; return 0;
      }
    }
    else {
      cout << "IMPOSSIBLE" << endl;
      return 0;
    } 
  }
  else if (n == 2) {
    int m2 = m / 2;
    if (r == 0) {
      for (int i = 0; i < m; i++) {
        poster[0][i] = 'a' + (i % 3);
        if (i < m2 || (m - i) <= m2)
          poster[1][i] = 'a' + (i % 3);
        else
          poster[1][i] = 'b' + (i % 3);
      }
    }
    else if (r == 1) {
      for (int i = 0; i < m; i++) {
        for (int i = 0; i < m; i++) {
          poster[0][i] = 'a';
          if (i < m2 || (m - i) <= m2)
            poster[1][i] = 'a';
          else
            poster[1][i] = 'b' + (i % 3);
        }
      }
    }
    else if (r == 2) {
      for (int i = 0; i < m; i++) {
        if (i < m2 || (m - i) <= m2) {
          poster[0][i] = 'a';
          poster[1][i] = 'a';
        }
        else {
          poster[0][i] = 'b';
          poster[1][i] = 'c';
        }
      }
    }
    
  }
  else {
    cout << "IMPOSSIBLE" << endl;
    return 0;
  }
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      cout.put(poster[i][j]);
    }
    cout << endl;
  }
}