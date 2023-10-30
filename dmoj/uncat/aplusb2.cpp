#include <algorithm>
#include <charconv>
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
using word_t  = uint64_t;
using dword_t = unsigned __int128;
using ullong  = unsigned long long;

constexpr int word_digits = std::numeric_limits<word_t>::digits10;
// 10^word_digits
constexpr word_t word_p10  = ([]() -> word_t {
  word_t x = 1;
  for (int i = 0; i < word_digits; i++)
    x *= 10;
  return x;
})();
constexpr word_t word_max  = std::numeric_limits<word_t>::max();
constexpr word_t word_bits = std::numeric_limits<word_t>::digits;

struct bigint {
  std::vector<word_t> words {0};
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

inline void trim_x(std::vector<word_t>& a) {
  while (a.back() == 0 && a.size() > 1)
    a.pop_back();
}

inline void cadd_xw(std::vector<word_t>& a, word_t b) {
  if (a.size() == 0) {
    a.push_back(b);
    return;
  }
  bool carry = _addcarry_u64(false, a[0], b, (ullong*) &a[0]);
  for (size_t i = 1; i < a.size() && carry; i++) {
    carry = _addcarry_u64(carry, a[i], 0, (ullong*) &a[i]);
  }
  if (carry)
    a.push_back(1);
}

inline void cadd_xx(std::vector<word_t>& a, const std::vector<word_t>& b) {
  if (b.size() > a.size())
    a.resize(b.size());

  bool carry = false;
  for (size_t i = 0; i < a.size(); i++) {
    carry =
      _addcarry_u64(carry, a[i], (i < b.size() ? b[i] : 0), (ullong*) &a[i]);
  }
  if (carry)
    a.push_back(1);
}

inline void ccomp_x(std::vector<word_t>& a) {
  bool carry = true;
  for (size_t i = 0; i < a.size(); i++) {
    carry = _addcarry_u64(carry, ~a[i], 0, (ullong*) &a[i]);
  }
}

// return true if the sign flips.
inline bool csub_xx(std::vector<word_t>& a, const std::vector<word_t>& b) {
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
inline bool crsub_xx(std::vector<word_t>& a, const std::vector<word_t>& b) {
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

inline void cmul_xw(std::vector<word_t>& a, word_t b) {
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
inline word_t cdiv_xw(std::vector<word_t>& a, word_t b) {
  word_t rem = 0;
  for (size_t i = a.size() - 1; i != 0; i--) {
    rem = _div_u64(rem, a[i], b, &a[i]);
  }
  rem = _div_u64(rem, a[0], b, &a[0]);
  trim_x(a);
  return rem;
}

// like strcmp: return <0 if a<b, >0 if a>b, and ==0 if a==b.
inline int cmp_xx(const std::vector<word_t>& a, const std::vector<word_t>& b) {
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

// SIGNED MATH ROUTINES
// ====================
inline void signfix_y(bigint& a) {
  if (a.words.size() == 1 && a.words[0] == 0)
    a.sign = false;
}
inline void cadd_yy(bigint& a, const bigint& b) {
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
}

// DEBUG ROUTINES
// ==============

inline std::ostream& operator<<(
  std::ostream& out, const std::vector<word_t>& v) {
  auto flags = out.flags();

  out << '[' << std::hex;
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
    0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,
    4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,
    9,  9,  10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14,
    14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18,
    19,
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

inline bigint parse_ys(std::string_view token) {
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
    std::from_chars(it, it + lead_len, chunk);
    res.words[0] = chunk;
    it += lead_len;
  }

  // trailing chunks
  for (size_t i = 0; i < n_chunks; i++) {
    std::from_chars(it, it + word_digits, chunk);

    cmul_xw(res.words, word_p10);
    cadd_xw(res.words, chunk);
    it += word_digits;
  }

  signfix_y(res);
  return res;
}

inline std::string stream_sy(bigint& a) {
  std::vector<word_t> chunks;
  do {
    word_t rem = cdiv_xw(a.words, word_p10);
    chunks.push_back(rem);
  } while (a.words.size() > 1 || a.words[0] != 0);
  if (chunks.empty())
    chunks.push_back(0);

  auto it   = chunks.rbegin();
  size_t dc = std::max(ilog10(*it), (size_t) 1);

  std::string out(a.sign + dc + (chunks.size() - 1) * word_digits, '0');
  char* optr = out.data();
  char* eptr = out.data() + out.size();

  if (a.sign) {
    optr[0] = '-';
    ++optr;
  }

  auto res = std::to_chars(optr, eptr, *it);
  ++it;
  optr = res.ptr;

  while (it != chunks.rend()) {
    dc  = std::max(ilog10(*it), (size_t) 1);
    res = std::to_chars(optr + word_digits - dc, eptr, *it);
    ++it;
    optr = res.ptr;
  }

  return out;
}

inline std::istream& operator>>(std::istream& in, bigint& bi) {
  std::string token;
  in >> token;
  bi = parse_ys(token);
  return in;
}

inline std::ostream& operator<<(std::ostream& out, const bigint& bi) {
  bigint copy        = bi;
  std::string buffer = stream_sy(copy);
  return out << buffer;
}

inline std::ostream& operator<<(std::ostream& out, bigint&& bi) {
  std::string buffer = stream_sy(bi);
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
    cadd_yy(x, y);
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
    cadd_yy(x, y);
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