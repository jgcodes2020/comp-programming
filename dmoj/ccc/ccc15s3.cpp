#include <bitset>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using namespace std;

int solve(int p) {
  map<u32, u32> gs;

  for (int i = 0; i < p; i++) {
    u32 g_i;
    cin >> g_i;

    // check if there is a range starting above g_i
    if (auto it = gs.lower_bound(g_i); it != gs.end()) {
      if (it->second >= g_i + 2) {
        // the range ends before g_i, start a new range or merge to one below
        if (auto it = gs.find(g_i - 1); it != gs.end()) {
          // merge to range below
          auto lo = it->second;
          gs.erase(it);
          gs.emplace(g_i, lo);
        }
        else {
          // start new range
          gs.emplace(g_i, g_i);
        }
      }
      else if (it->second == 1) {
        // everything from g_i -> 1 is occupied, nowhere to go
        return i;
      }
      else {
        // add to the current range, there's free space below
        auto val = --it->second;
        // merge with any range below
        if (auto it2 = gs.find(val - 1); it2 != gs.end()) {
          it->second = it2->second;
          gs.erase(it2);
        }
      }
    }
    else {
      if (auto it = gs.find(g_i - 1); it != gs.end()) {
        // merge current element with a range below
        auto lo = it->second;
        gs.erase(it);
        gs.emplace(g_i, lo);
      }
      else {
        // start new range
        gs.emplace(g_i, g_i);
      }
    }
  }
  return p;
}

int main() {
  u32 g, p;
  cin >> g >> p;

  int res = solve(p);

  cout << res << '\n';
}