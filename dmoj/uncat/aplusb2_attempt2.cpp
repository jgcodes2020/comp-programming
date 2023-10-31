#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <ostream>
#include <ratio>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <immintrin.h>
#include <x86intrin.h>

#define EXPR(args, expr) \
  [] args {              \
    return (expr);       \
  }

using namespace std;
using ullong = unsigned long long;

using word_t  = uint64_t;
using dword_t = unsigned __int128;

constexpr int word_digits = numeric_limits<word_t>::digits10;
// 10^word_digits
constexpr word_t word_p10  = ([]() -> word_t {
  word_t x = 1;
  for (int i = 0; i < word_digits; i++)
    x *= 10;
  return x;
})();
constexpr word_t word_max  = numeric_limits<word_t>::max();
constexpr word_t word_bits = numeric_limits<word_t>::digits;

using wvec_t = vector<word_t>;

class wvec_range {
private:
  wvec_t::const_iterator m_begin;
  wvec_t::const_iterator m_end;

public:
  wvec_range(const wvec_t& v, size_t begin = 0) noexcept :
    m_begin(v.cbegin() + begin), m_end(v.cend()) {}
  wvec_range(const wvec_t& v, size_t begin, size_t end) noexcept :
    m_begin(v.cbegin() + begin), m_end(v.cbegin() + end) {}

  wvec_range(const wvec_range&)            = default;
  wvec_range& operator=(const wvec_range&) = default;

  wvec_range(const wvec_range& v, size_t begin) :
    m_begin(v.m_begin + begin), m_end(v.m_end) {}
  wvec_range(const wvec_range& v, size_t begin, size_t end) :
    m_begin(v.m_begin + begin), m_end(v.m_begin + end) {}

  const word_t& operator[](size_t i) const noexcept { return m_begin[i]; }

  size_t size() { return m_end - m_begin; }

  wvec_t::const_iterator begin() const noexcept { return m_begin; }
  wvec_t::const_iterator end() const noexcept { return m_end; }
  wvec_t::const_iterator cbegin() const noexcept { return m_begin; }
  wvec_t::const_iterator cend() const noexcept { return m_end; }

  reverse_iterator<wvec_t::const_iterator> rbegin() const noexcept {
    return make_reverse_iterator(m_begin);
  }
  reverse_iterator<wvec_t::const_iterator> rend() const noexcept {
    return make_reverse_iterator(m_end);
  }
  reverse_iterator<wvec_t::const_iterator> crbegin() const noexcept {
    return make_reverse_iterator(m_begin);
  }
  reverse_iterator<wvec_t::const_iterator> crend() const noexcept {
    return make_reverse_iterator(m_end);
  }
};

struct bigint {
  wvec_t words {0};
  bool sign = false;
};

// ASSEMBLY INTRINSICS
// =============

[[nodiscard]] inline word_t _mul_u64(word_t a, word_t b, word_t* lo) {
  word_t _hi, _lo;

  __asm__("mulq %[b]" : "=d"(_hi), "=a"(_lo) : "a"(a), [b] "r"(b));

  *lo = _lo;
  return _hi;
}

[[nodiscard]] inline word_t _div_u64(
  word_t numhi, word_t numlo, word_t den, word_t* quot) {
  word_t _quot, _rem;
  __asm__("divq %[den]"
          : "=a"(_quot), "=d"(_rem)
          : "d"(numhi), "a"(numlo), [den] "r"(den));

  *quot = _quot;
  return _rem;
}

// UNSIGNED MATH ROUTINES
// ======================

inline void trim_x(wvec_t& a) {
  while (a.back() == 0 && a.size() > 1)
    a.pop_back();
}

inline void cadd_xw(wvec_t& a, word_t b) {
  if (a.size() == 0) {
    a.push_back(b);
    return;
  }
  bool carry = _addcarry_u64(false, a[0], b, (ullong*) &a[0]);
  for (size_t i = 1; i < a.size(); i++) {
    carry = _addcarry_u64(carry, a[i], 0, (ullong*) &a[i]);
    // this is the only carry that will propagate, we can stop early
    // if it stops going
    if (!carry)
      return;
  }
  if (carry)
    a.push_back(1);
}

inline void add_xx(wvec_range a, wvec_range b, wvec_t& r) {
  r.clear();
  r.reserve(max(a.size(), b.size()));

  bool carry = false;
  word_t rw = 0;
  for (size_t i = 0; i < r.size(); i++) {
    carry = _addcarry_u64(
      carry, a[i], (i < b.size() ? b[i] : 0), (ullong*) &rw);
    r.push_back(rw);
  }
  if (carry)
    r.push_back(1);
}

inline void cadd_xx(wvec_t& a, wvec_range b, size_t offb = 0) {
  if (b.size() + offb > a.size())
    a.resize(b.size() + offb);

  bool carry = false;
  for (size_t i = offb; i < a.size(); i++) {
    carry = _addcarry_u64(
      carry, a[i], ((i - offb) < b.size() ? b[i - offb] : 0), (ullong*) &a[i]);
  }
  if (carry)
    a.push_back(1);
}

inline void ccomp_x(wvec_t& a) {
  bool carry = true;
  for (size_t i = 0; i < a.size(); i++) {
    carry = _addcarry_u64(carry, ~a[i], 0, (ullong*) &a[i]);
  }
}

// return true if the sign flips.
inline bool csub_xx(wvec_t& a, wvec_range b) {
  if (b.size() > a.size())
    a.resize(b.size());

  bool carry = false;
  for (size_t i = 0; i < a.size(); i++) {
    carry =
      _subborrow_u64(carry, a[i], (i < b.size() ? b[i] : 0), (ullong*) &a[i]);
  }

  if (carry) {
    ccomp_x(a);
  }
  trim_x(a);
  return carry;
}

// like csub_xx but reverses the subtraction.
inline bool crsub_xx(wvec_t& a, wvec_range b) {
  if (b.size() > a.size())
    a.resize(b.size());

  bool carry = false;
  for (size_t i = 0; i < a.size(); i++) {
    carry =
      _subborrow_u64(carry, (i < b.size() ? b[i] : 0), a[i], (ullong*) &a[i]);
  }

  if (carry) {
    ccomp_x(a);
  }
  trim_x(a);
  return carry;
}

inline void mul_xw(wvec_range a, word_t b, wvec_t& r) {
  r.clear();
  r.reserve(a.size());
  word_t carry = 0;
  for (size_t i = 0; i < a.size(); i++) {
    word_t hi, lo;

    hi = _mul_u64(a[i], b, &lo);
    hi += _addcarry_u64(false, lo, carry, (ullong*) &lo);

    r.push_back(lo);
    carry = hi;
  }
  if (carry > 0)
    r.push_back(carry);
}

inline void cmul_xw(wvec_t& a, word_t b) {
  word_t carry = 0;
  for (size_t i = 0; i < a.size(); i++) {
    word_t hi, lo;

    hi = _mul_u64(a[i], b, &lo);
    hi += _addcarry_u64(false, lo, carry, (ullong*) &lo);

    a[i]  = lo;
    carry = hi;
  }
  if (carry > 0)
    a.push_back(carry);
}

// returns remainder.
inline word_t cdiv_xw(wvec_t& a, word_t b) {
  word_t rem = 0;
  for (size_t i = a.size() - 1; i != 0; i--) {
    rem = _div_u64(rem, a[i], b, &a[i]);
  }
  rem = _div_u64(rem, a[0], b, &a[0]);
  trim_x(a);
  return rem;
}

// like strcmp: return <0 if a<b, >0 if a>b, and ==0 if a==b.
inline int cmp_xx(wvec_range a, wvec_range b) {
  if (a.size() < b.size())
    return -1;
  if (a.size() > b.size())
    return 1;

  auto [ma, mb] = mismatch(a.rbegin(), a.rend(), b.rbegin());
  if (ma == a.rend())
    return 0;
  if (*ma < *mb)
    return -1;
  else
    return 1;
}

// MULTIPLICATION
// ==============

inline constexpr size_t mul_toom2_min = 20;

inline void mul_base_xx(wvec_range a, wvec_range b, wvec_t& r) {
  r.clear();
  r.push_back(0);

  wvec_t t;
  t.reserve(a.size() + b.size());
  for (size_t i = 0; i < b.size(); i++) {
    mul_xw(a, b[i], t);
    cadd_xx(r, t, i);
  }
}

inline void mul_toom2_xx(wvec_range a, wvec_range b, wvec_t& r) {
  const size_t chunk_size = (max(a.size(), b.size()) + 1) / 2;
  if (min(a.size(), b.size()) < chunk_size || min(a.size(), b.size()) < mul_toom2_min) {
    mul_base_xx(a, b, r);
    return;
  }
  
  r.clear();
  r.push_back(0);
  
  wvec_t t0, t1, t2;
  t0.reserve(2 * chunk_size);
  t1.reserve(2 * chunk_size);
  t2.reserve(2 * chunk_size);
  
  wvec_range a0 {a, 0, chunk_size};
  wvec_range a1 {a, chunk_size, a.size() - chunk_size};
  wvec_range b0 {b, 0, chunk_size};
  wvec_range b1 {b, chunk_size, b.size() - chunk_size};
  
  // t1 = (a0 + a1) * (b0 + b1)
  add_xx(a0, a1, t0);
  add_xx(b0, b1, t2);
  mul_toom2_xx(t0, t2, t1);
  
  // t0 = a0 * b0, t2 = a1 * b1
  mul_toom2_xx(a0, b0, t0);
  mul_toom2_xx(a1, b1, t2);
  
  // t1 = t1 - t0 - t2
  csub_xx(t1, t0);
  csub_xx(t1, t2);
  
  // combine digits
  cadd_xx(r, t0);
  cadd_xx(r, t1, chunk_size);
  cadd_xx(r, t2, chunk_size * 2);
}
// SIGNED MATH ROUTINES
// ====================
inline void signfix_y(bigint& a) {
  if (a.words.size() == 1 && a.words[0] == 0)
    a.sign = false;
}
inline bigint& operator+=(bigint& a, const bigint& b) {
  if (a.sign ^ b.sign) {
    int cmp = cmp_xx(a.words, b.words);
    if (cmp < 0) {
      crsub_xx(a.words, b.words);
      a.sign = !a.sign;
    }
    else if (cmp == 0) {
      a.words.resize(1, 0);
      a.words[0] = 0;
    }
    else {
      csub_xx(a.words, b.words);
    }
  }
  else
    cadd_xx(a.words, b.words);

  signfix_y(a);
  return a;
}

// DEBUG ROUTINES
// ==============

inline ostream& operator<<(ostream& out, const wvec_t& v) {
  auto flags = out.flags();
  out.setf(ios::hex | ios::uppercase);

  out << '[';
  if (v.size() > 0) {
    out << setw(16) << setfill('0') << v[0];
    for (size_t i = 1; i < v.size(); i++)
      out << ", " << setw(16) << setfill('0') << v[i];
  }
  out << ']';
  out.flags(flags);
  return out;
}

// SERIALIZATION ROUTINES
// ======================

inline size_t ilog2(word_t x) {
  return (x == 0) ? 0 : word_bits - __builtin_clzl(x);
}

inline size_t ilog10(word_t x) {
  // precalculated: \log_{10}(2^x)
  constexpr size_t approx_table[] {
    0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,
    5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9,
    10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15,
    15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19,
  };
  // precalculated: 10^x
  constexpr word_t pow10[] {
    1U,
    10U,
    100U,
    1'000U,
    10'000U,
    100'000U,
    1'000'000U,
    10'000'000U,
    100'000'000U,
    1'000'000'000U,
    10'000'000'000U,
    100'000'000'000U,
    1'000'000'000'000U,
    10'000'000'000'000U,
    100'000'000'000'000U,
    1'000'000'000'000'000U,
    10'000'000'000'000'000U,
    100'000'000'000'000'000U,
    1'000'000'000'000'000'000U,
    10'000'000'000'000'000'000U,
  };

  size_t guess = approx_table[ilog2(x)];
  return guess + (x >= pow10[guess]);
}

inline bigint parse_ys(string_view token) {
  bigint res;

  auto it = token.data();

  if (*it == '-') {
    res.sign = true;
    ++it;
  }

  const size_t mag_len  = token.data() + token.size() - it;
  const size_t n_chunks = mag_len / word_digits;
  const size_t lead_len = mag_len % word_digits;

  word_t chunk;

  // leading chunk
  if (lead_len > 0) {
    from_chars(it, it + lead_len, chunk);
    res.words[0] = chunk;
    it += lead_len;
  }

  // trailing chunks
  for (size_t i = 0; i < n_chunks; i++) {
    from_chars(it, it + word_digits, chunk);

    cmul_xw(res.words, word_p10);
    cadd_xw(res.words, chunk);
    it += word_digits;
  }

  signfix_y(res);
  return res;
}

inline string stream_sy(bigint& a) {
  wvec_t chunks;
  do {
    word_t rem = cdiv_xw(a.words, word_p10);
    chunks.push_back(rem);
  } while (a.words.size() > 1 || a.words[0] != 0);
  if (chunks.empty())
    chunks.push_back(0);

  auto it   = chunks.rbegin();
  size_t dc = max(ilog10(*it), (size_t) 1);

  string out(a.sign + dc + (chunks.size() - 1) * word_digits, '0');
  char* optr = out.data();
  char* eptr = out.data() + out.size();

  if (a.sign) {
    optr[0] = '-';
    ++optr;
  }

  auto res = to_chars(optr, eptr, *it);
  ++it;
  optr = res.ptr;

  while (it != chunks.rend()) {
    dc  = max(ilog10(*it), (size_t) 1);
    res = to_chars(optr + word_digits - dc, eptr, *it);
    ++it;
    optr = res.ptr;
  }

  return out;
}

inline istream& operator>>(istream& in, bigint& bi) {
  string token;
  in >> token;
  bi = parse_ys(token);
  return in;
}

inline ostream& operator<<(ostream& out, const bigint& bi) {
  bigint copy   = bi;
  string buffer = stream_sy(copy);
  return out << buffer;
}

inline ostream& operator<<(ostream& out, bigint&& bi) {
  string buffer = stream_sy(bi);
  return out << buffer;
}

// TESTING ROUTINES
inline void aplusb2_dmoj_solution() {
  int c;
  cin >> c;

  bigint x, y;
  for (int i = 0; i < c; i++) {
    cin >> x;
    cin >> y;
    x += y;
    cout << std::move(x) << '\n';
  }
  cout << flush;
}

inline void aplusb2_test() {
  int c;
  cin >> c;

  bigint x, y;
  for (int i = 0; i < c; i++) {
    cin >> x;
    cout << "x = (" << x.sign << ", " << x.words << ")\n";
    cin >> y;
    cout << "y = (" << y.sign << ", " << y.words << ")\n";
    x += y;
    cout << "x + y = (" << x.sign << ", " << x.words << ")\n";
    cout << std::move(x) << '\n';
  }
  cout << flush;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  cout.setf(ios_base::boolalpha);

  aplusb2_dmoj_solution();
}