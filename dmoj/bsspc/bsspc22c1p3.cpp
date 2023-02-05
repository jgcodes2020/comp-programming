#include <bits/stdc++.h>
#include <functional>
#include <unordered_set>
#include <vector>
using namespace std;


struct pos {
  int x, y;
};

bool operator==(pos x, pos y) {
  return x.x == y.x && x.y == y.y;
}
namespace std {
  template <>
  struct hash<pos> {
    size_t operator()(pos p) const {
      return p.x ^ p.y;
    }
  };
}

void check(unordered_set<pos>& dst, const vector<pos>& src, const pos& p) {
  uint cnt = 0;
  for (pos f : src) {
    int dx = f.x - p.x, dy = f.y - p.y;
    if (abs(dx) <= 1 && abs(dy) <= 1 && (dx != 0 || dy != 0)) {
      cnt++;
    }
  }
  if (cnt >= 3) {
    dst.insert(p);
    cout << "(" << p.x << ", " << p.y << ")\n";
  }
}

char room[1002][1002];

int main() {
  memset(room, 0, sizeof(room));
  
  int n, m, k;
  cin >> n >> m >> k;
  
  vector<pos> fnds;
  int in_a, in_b;
  for (uint i = 0; i < k; i++) {
    cin >> in_a >> in_b;
    room[in_a][in_b] = 'F';
    fnds.push_back({in_a, in_b});
  }
  
  uint cnt = 0;
  for (uint i = 0; i < fnds.size(); i++) {
    pos p = fnds[i];
    for (pos cnd : {
      pos {p.x + 1, p.y + 1},
      pos {p.x + 1, p.y + 0},
      pos {p.x + 1, p.y - 1},
      pos {p.x - 1, p.y + 1},
      pos {p.x - 1, p.y + 0},
      pos {p.x - 1, p.y - 1},
      pos {p.x + 0, p.y + 1},
      pos {p.x + 0, p.y - 1},
    }) {
      if (room[cnd.x][cnd.y])
        continue;
      
      uint ccnt = 0;
      for (pos cnb : {
        pos {cnd.x + 1, cnd.y + 1},
        pos {cnd.x + 1, cnd.y + 0},
        pos {cnd.x + 1, cnd.y - 1},
        pos {cnd.x - 1, cnd.y + 1},
        pos {cnd.x - 1, cnd.y + 0},
        pos {cnd.x - 1, cnd.y - 1},
        pos {cnd.x + 0, cnd.y + 1},
        pos {cnd.x + 0, cnd.y - 1},
      }) {
        ccnt += uint(room[cnb.x][cnb.y] == 'F');
      }
      if (ccnt >= 3) {
        room[cnd.x][cnd.y] = 'S';
        clog << cnd.x << ' ' << cnd.y << '\n';
        cnt++;
      }
    }
  }
  
  cout << cnt << '\n';
}
