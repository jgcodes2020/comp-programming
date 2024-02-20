#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>
using namespace std;

char b[8][8];
vector<pair<int, int>> sp;

inline std::ostream& print_vm(std::ostream& out, uint64_t v) {
  auto flags = out.flags();
  out << uppercase << hex << setw(16) << setfill('0') << v;
  out.flags(flags);
  return out;
}

inline optional<uint64_t> can_move(int x, int y, int dx, int dy, uint64_t v) {
  int cx = x + dx, cy = y + dy;
  int ex = cx + dx, ey = cy + dy;
  
  if (!(0 <= ex && ex < 8) || !(0 <= ey && ey < 8))
    return nullopt;
  if ((v >> (cx * 8 + cy)) & 1)
    return nullopt;
  
  uint64_t nv = v | (uint64_t(1) << (cx * 8 + cy));
  return (b[cx][cy] == 'B' && b[ex][ey] == '.') ? optional<uint64_t>(nv) : nullopt;
}

int visit(int x, int y, uint64_t v = 0) {
  int max = 0;
  if (auto vn = can_move(x, y, -1, -1, v)) {
    max = std::max(max, visit(x - 2, y - 2, *vn) + 1);
  }
  if (auto vn = can_move(x, y, -1, +1, v)) {
    max = std::max(max, visit(x - 2, y + 2, *vn) + 1);
  }
  if (auto vn = can_move(x, y, +1, -1, v)) {
    max = std::max(max, visit(x + 2, y - 2, *vn) + 1);
  }
  if (auto vn = can_move(x, y, +1, +1, v)) {
    max = std::max(max, visit(x + 2, y + 2, *vn) + 1);
  }
    
  return max;
} 


int main() {
  for (int i = 0; i < 8; i++) {
    cin.read(b[i], 8);
    cin.ignore(1);
  }
  // . = empty, A = me, B = opponent
  
  int max = 0;
  for (int i = 7; i >= 0; i--) {
    for (int j = 0; j < 8; j++) {
      if (b[i][j] == 'A')
        max = std::max(max, visit(i, j));
    }
  }
  
  cout << max << endl;
}