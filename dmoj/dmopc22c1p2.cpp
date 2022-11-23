/*
jgcodes's dead simple CP template
*/

#include <bits/stdc++.h>
#include <queue>

// Rust-style ints
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using isz = size_t;

using c8 = char;

template <class T>
using vec = std::vector<T>;
template <class T, size_t N>
using arr = std::array<T, N>;
template <class K, class T>
using map = std::unordered_map<K, T>;
template <class K, class T>
using smap = std::map<K, T>;
template <class T>
using set = std::unordered_set<T>;
template <class T>
using sset = std::set<T>;

template <class T>
using stk = std::stack<T, std::vector<T>>;
template <class T>
using que = std::queue<T>;
template <class T>
using pque = std::priority_queue<T>;

template <class T, class U>
using pr = std::pair<T, U>;

// scanf, but it will throw on error, crashing your program
// instead of causing UB
template <class... Ts>
inline void scanf2(const char* format, Ts&... args) {
  int res = scanf(format, std::addressof(args)...);
  if ((size_t) res < sizeof...(Ts))
    throw std::runtime_error("scanf didn't scan all args");
}

int main() {
  vec<u32> hc;
  map<u32, vec<u32>> g;
  u32 n, m;

  scanf2("%d %d", n, m);

  hc.reserve(n + 1);
  hc.push_back(0);

  for (u32 i = 0; i < n; i++) {
    u32 x;
    scanf2("%d ", x);
    hc.push_back(x);
  }
  for (u32 i = 0; i < m; i++) {
    u32 p, q;
    scanf2("%d %d\n", p, q);
    g[p].push_back(q);
    g[q].push_back(p);
  }

  // hat color -> {person: distance;...}
  map<u32, map<u32, u32>> s_hm;
  map<u32, map<u32, u32>> k_hm;
  
  // BFS for Shirogane
  {
    set<u32> vis;
    que<pr<u32, u32>> s;

    s.emplace(1, 0);
    while (!s.empty()) {
      auto el = s.front();
      s.pop();
      u32 vl  = el.first;
      u32 dep = el.second;

      vis.insert(vl);
      // add to hat map
      s_hm[hc[vl]].emplace(vl, dep);

      for (u32 p : g[vl]) {
        if (!vis.count(p)) {
          s.emplace(p, dep + 1);
        }
      }
    }
  }
  // BFS for Kaguya
  {
    set<u32> vis;
    que<pr<u32, u32>> s;

    s.emplace(n, 0);
    while (!s.empty()) {
      auto el = s.front();
      s.pop();
      u32 vl  = el.first;
      u32 dep = el.second;

      vis.insert(vl);
      // add to hat map
      k_hm[hc[vl]].emplace(vl, dep);

      for (u32 p : g[vl]) {
        if (!vis.count(p)) {
          s.emplace(p, dep + 1);
        }
      }
    }
  }
  
  u32 n_min = -1;

  for (const auto& se : s_hm) {
    auto ki = k_hm.find(se.first);
    if (ki == k_hm.end())
      continue;

    const auto& ke = *ki;
    {
      // Find min and 2nd min for both people

      auto si    = se.second.begin();
      u32 sd_min = si->second, sd_2min = sd_min;
      u32 sd_minp = 0, sd_2minp = 0;
      for (auto i = si; i != se.second.end(); i++) {
        if (i->second < sd_min) {
          sd_2min  = sd_min;
          sd_2minp = sd_minp;
          sd_min   = i->second;
          sd_minp  = i->first;
        }
      }

      auto ki    = ke.second.begin();
      u32 kd_min = ki->second, kd_2min = kd_min;
      u32 kd_minp = 0, kd_2minp = 0;
      for (auto i = ki; i != se.second.end(); i++) {
        if (i->second < kd_min) {
          kd_2min  = kd_min;
          kd_2minp = kd_minp;
          kd_min   = i->second;
          kd_minp  = i->first;
        }
      }

      // conflict, choose 2nd best
      if (sd_minp == kd_minp) {
        // exclusion case
        bool s_no2p = sd_2minp == 0;
        bool k_no2p = kd_2minp == 0;
        if (s_no2p && k_no2p) {
          // NOTHING
        }
        // exclusion case
        else if (s_no2p) {
          n_min = std::min(n_min, kd_2min + sd_min);
        }
        else if (k_no2p) {
          n_min = std::min(n_min, sd_2min + kd_min);
        }
        // find cheaper 2nd best
        else {
          u32 s_cost = sd_2min - sd_min;
          u32 k_cost = kd_2min - kd_min;
          if (s_cost > k_cost) {
            n_min = std::min(n_min, kd_2min + sd_min);
          }
          else {
            n_min = std::min(n_min, sd_2min + kd_min);
          }
        }
      }
      else {
        n_min = std::min(n_min, sd_min + kd_min);
      }
    }
  }
  
  printf("%d\n", i32(n_min));
}