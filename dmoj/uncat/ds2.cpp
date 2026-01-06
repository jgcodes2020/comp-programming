#include <bits/stdc++.h>

using namespace std;

using u32 = uint32_t;

struct dsu_node {
  u32 parent;
  u32 size;
};

class disjoint_set {
public:
  disjoint_set(u32 size) {
    m_nodes.reserve(size + 1);
    for (u32 i = 0; i < size + 1; i++) {
      m_nodes.emplace_back(i, 1);
    }
  }

  // Finds the root containing a given node.
  u32 find(u32 x) {
    assert(x != 0);

    // traverse up to the root
    u32 root = x;
    while (m_nodes[root].parent != root) {
      root = m_nodes[root].parent;
    }

    // flatten paths up to root
    u32 curr = x;
    while (curr != root) {
      u32 parent = m_nodes[curr].parent;
      m_nodes[curr].parent = root;
      curr = parent;
    }

    return root;
  }

  // Merges the two given nodes, returning true if a merge actually occurred.
  bool merge(u32 x, u32 y) {
    assert(x != 0);
    assert(y != 0);

    // find roots
    u32 xr = find(x);
    u32 yr = find(y);

    // our two nodes might be connected, in this case do nothing
    if (xr == yr) {
      return false;
    }

    // ensure xr is the larger tree
    if (m_nodes[xr].size < m_nodes[yr].size) {
      std::swap(xr, yr);
    }

    // link yr to xr, updating size information
    m_nodes[yr].parent = xr;
    m_nodes[xr].size += m_nodes[yr].size;
    return true;
  }

private:
  std::vector<dsu_node> m_nodes;
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  u32 n, m;
  cin >> n >> m;

  std::vector<u32> mst_edges;
  disjoint_set dsu(n);

  // Edges are pre-sorted by weight.
  // Add all edges that connect previously disconnected areas.
  for (u32 i = 1; i <= m; i++) {
    u32 u, v;
    cin >> u >> v;

    if (dsu.merge(u, v)) {
      mst_edges.push_back(i);
    }
  }

  // Confirm that the whole graph is connected.
  u32 final_root = dsu.find(1);
  bool connected = true;
  for (u32 i = 2; i <= n; i++) {
    if (dsu.find(i) != final_root) {
      connected = false;
      break;
    }
  }

  if (connected) {
    for (u32 edge : mst_edges) {
      cout << edge << '\n';
    }
  }
  else {
    cout << "Disconnected Graph\n";
  }
  cout.flush();

  return 0;
}