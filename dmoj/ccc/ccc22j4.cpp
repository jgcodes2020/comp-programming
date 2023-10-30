#include <bits/stdc++.h>
#include <unordered_map>
#include <vector>

using namespace std;

struct group {
  string pa, pb, pc;
};

int main() {
  unordered_map<string, unordered_set<string>> tmap;
  unordered_map<string, unordered_set<string>> smap;
  vector<group> groups;
  {
    std::string pa, pb, pc;
    
    int n;
    // together
    cin >> n;
    for (int i = 0; i < n; i++) {
      cin >> pa >> pb;
      tmap[pa].insert(pb);
      tmap[pb].insert(pa);
    }
    // separate
    cin >> n;
    for (int i = 0; i < n; i++) {
      cin >> pa >> pb;
      smap[pa].insert(pb);
      smap[pb].insert(pa);
    }
    // groups
    cin >> n;
    for (int i = 0; i < n; i++) {
      cin >> pa >> pb >> pc;
      groups.push_back({pa, pb, pc});
    }
  }
  // for each group do test
  {
    for (const group& g : groups) {
      if (tmap[g.pa].count(g.pb) || tmap[g.pa].count(g.pc)) {
        
      }
      if (tmp)
    }
  }
}