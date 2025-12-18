#include <bits/stdc++.h>

using namespace std;

using u32 = uint32_t;
struct u32_edge {
  u32 v;
  u32 w;
};

constexpr auto cmp_weight = [](const u32_edge& a, const u32_edge& b) {
  return a.w > b.w;
};
using cmp_weight_t = decltype(cmp_weight);

inline u32 sat_add(u32 a, u32 b) {
  u32 sum = a + b;
  return (sum < a) ? UINT32_MAX : sum;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  vector<vector<u32_edge>> graph;
  size_t n = 0, m = 0;

  cin >> n >> m;
  graph.resize(n + 1);

  for (size_t i = 0; i < m; i++) {
    u32 u = 0, v = 0, w = 0;
    cin >> u >> v >> w;
    graph[u].emplace_back(v, w);
    graph[v].emplace_back(u, w);
  }
  
  vector<u32> dist;

  // prepare dist array; set node 1 distance to 0; then everything else to
  // maximum
  dist.reserve(n + 1);
  dist.resize(2, 0);
  dist.resize(n + 1, UINT32_MAX);

  for (size_t i = 0; i < n - 1; i++) {
    // relax edges
    for (u32 curr_v = 1; curr_v <= n; curr_v++) {
      for (auto edge : graph[curr_v]) {
        // check if the path from here is
        // shorter; record it if it is
        u32 from_curr = sat_add(dist[curr_v], edge.w);
        if (from_curr < dist[edge.v]) {
          dist[edge.v] = from_curr;
        }
      }
    }
  }

  for (u32 i = 1; i <= n; i++) {
    u32 dist_i = dist[i];
    if (dist_i == UINT32_MAX)
      cout << "-1";
    else
      cout << dist_i;
    cout << '\n';
  }
  cout << flush;
}