#include <algorithm>
#include <array>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <iostream>
using namespace std;

struct u32_edge {
  int src;
  double w;
};
constexpr size_t max_n = 1e5 + 3;

int n = 0, m = 0;
array<vector<int>, max_n> gd = {}; // down
array<vector<u32_edge>, max_n> gu = {}; // up (with weights)
array<double, max_n> pval = {};
array<double, max_n> pacc = {};
array<double, max_n> fval = {};

array<uint8_t, max_n> top_mask = {};
vector<int> top_list = {};

void top_visit(int n) {
  if (top_mask[n] == 2)
    return;
  if (top_mask[n] == 1)
    throw std::runtime_error("CYCLE!!");
  
  top_mask[n] = 1;
  for (auto& e : gd[n]) {
    top_visit(e);
  }
  top_mask[n] = 2;
  top_list.push_back(n);
}

int main() {
  #if 0
  cin >> n;
  for (int i = 1; i <= n; i++) {
    int f, p, u, v;
    cin >> f >> p;
    pval[i] = p;
    if (f == -1) {
      cin >> u;
      if (u != 0) {
        gd[i].emplace_back(u);
        gu[u].push_back({i, 1.0});
      }
    }
    else {
      cin >> u >> v;
      double m = f / 100.0;
      
      gd[i].emplace_back(u);
      gd[i].emplace_back(v);
      
      gu[u].push_back({i, m});
      gu[v].push_back({i, 1.0 - m});
    }
  }
  #else
  cin >> n >> m;
  for (int i = 0; i < m; i++) {
    int u, v, f;
    cin >> u >> v >> f;
    
    double m = f / 100.0;
    // add stream segment
    gd[u].emplace_back(v);
    gu[v].push_back({u, m});
  }
  for (int i = 1; i <= n; i++) {
    double p;
    cin >> p;
    // set pollution value
    pval[i] = p;
  }
  #endif
  
  // top sort
  for (int i = 1; i <= n; i++) {
    top_visit(i);
  }
  std::reverse(top_list.begin(), top_list.end());
  
  // in top order, determine values
  for (int i : top_list) {
    if (gu[i].empty()) {
      // no incoming streams
      fval[i] = 1.0;
      pacc[i] = pval[i];
    }
    else {
      // total f (volume)
      // total p (pollution)
      double ft = 0.0, pt = 0.0;
      for (u32_edge& e : gu[i]) {
        double fe = fval[e.src] * e.w;
        ft += fe;
        pt += pacc[e.src] * fe;
      }
      // total volume flowing through junction
      fval[i] = ft;
      // total pollution at junction (weighted avg. of incoming + current)
      pacc[i] = (pt / ft) + pval[i];
    }
  }
  
  for (int i = 1; i <= n; i++) {
    cout << setprecision(std::numeric_limits<double>::digits10) << pacc[i] << '\n';
  }
}