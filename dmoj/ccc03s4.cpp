/*
jgcodes's dead simple CP template
*/

#include <bits/stdc++.h>
#include <cstring>
#include <ios>
#include <queue>
#include <string>

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

using str = std::string;

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

inline u32 extend_hash(u32 hash, char next) {
  return (31 * hash) ^ u32(next);
}

inline u32 hash_reduce8(u32 hash) {
  hash = (hash & 0xFFFF) ^ (hash >> 16);
  hash = (hash & 0xFF) ^ (hash >> 8);
  return hash;
}

inline str rdline() {
  str out;
  std::getline(std::cin, out);
  return out;
}


struct hnode {
  u32 hash;
  c8* begin;
  isz len;
};
using htbl = arr<vec<hnode>, 256>;

bool htbl_insert(htbl& ht, c8* begin, isz len, u32 hash) {
  auto& br = ht[hash_reduce8(hash)];
  for (const hnode& nd : br) {
    if (nd.len == len && 
      nd.hash == hash && 
      memcmp(begin, nd.begin, len) == 0) {
      return false;
    }
  }
  br.push_back({hash, begin, len});
  return true;
}

u32 run_case(str& in) {
  
  htbl ht = {};
  u32 cnt = 1;
  
  for (u32 i = 0; i < in.size(); i++) {
    // init FNV hash
    u32 ch = 0x811C9DC5;
    for (u32 j = i; j < in.size(); j++) {
      ch = (ch * 31) ^ u32(in[j]);
      cnt += u32(htbl_insert(ht, &in[i], j - i + 1, ch));
    }
  }
  return cnt;
}

int main() {
  map<str, u32> cache;
  
  u64 count = std::stoull(rdline());
  str line;
  u32 res;
  while (count-- > 0) {
    std::getline(std::cin, line);
    auto check = cache.find(line);
    if (check == cache.end()) {
      res = run_case(line);
      cache.emplace(line, res);
    }
    else {
      res = check->second;
    }
    
    printf("%u\n", res);
  }
}