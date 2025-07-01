#include <bits/stdc++.h>
#include <immintrin.h>

struct uint64_x2 {
  uint64_t a, b;
};

struct uint64_check {
  uint64_t value;
  bool valid;
};

namespace intr {
  [[nodiscard]] inline uint64_x2 _mulq(uint64_t a, uint64_t b) {
    uint64_t _hi, _lo;

    __asm__("mulq %[b]" : "=d"(_hi), "=a"(_lo) : "a"(a), [b] "r"(b));

    return {_hi, _lo};
  }

  [[nodiscard]] inline uint64_x2 _divq(
    uint64_t numhi, uint64_t numlo, uint64_t den) {
    uint64_t _quot, _rem;
    __asm__("divq %[den]"
            : "=a"(_quot), "=d"(_rem)
            : "d"(numhi), "a"(numlo), [den] "r"(den));

    return {_quot, _rem};
  }

  uint64_check sse42_parse16(const char* input) {
    const __m128i ASCII_ZERO  = _mm_set1_epi8('0');
    const __m128i NINE        = _mm_set1_epi8(9);
    const __m128i STAGE1_MASK = _mm_set1_epi16(0x010A);
    const __m128i STAGE2_MASK = _mm_set_epi16(1, 100, 1, 100, 1, 100, 1, 100);
    const __m128i STAGE3_MASK = _mm_set_epi16(0, 0, 0, 0, 1, 10'000, 1, 10'000);

    const uint64_t LOW_HALF = UINT64_C(0x0000'0000'FFFF'FFFF);
    const uint64_t ONE_E8   = 1e8;

    __m128i chunk = _mm_loadu_si128((const __m128i_u*) input);
    chunk         = _mm_sub_epi8(chunk, ASCII_ZERO);

    __m128i cmp = _mm_cmpgt_epi8(chunk, NINE);
    if (!_mm_test_all_zeros(cmp, cmp)) {
      return {0, false};
    }

    // merge groups of 2
    chunk = _mm_maddubs_epi16(chunk, STAGE1_MASK);
    // merge groups of 4
    chunk = _mm_madd_epi16(chunk, STAGE2_MASK);
    // merge groups of 8
    chunk = _mm_packus_epi32(chunk, chunk);
    chunk = _mm_madd_epi16(chunk, STAGE3_MASK);

    uint64_t data = _mm_cvtsi128_si64(chunk);
    return {(data & LOW_HALF) * ONE_E8 + (data >> 32), true};
  }
}  // namespace intr

namespace tbl {
  constexpr uint64_t log10_approx_table[] {
    0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,
    5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9,
    10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15,
    15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19,
  };

  constexpr uint64_t pow10[] = {
    UINT64_C(1),
    UINT64_C(10),
    UINT64_C(100),
    UINT64_C(1'000),
    UINT64_C(10'000),
    UINT64_C(100'000),
    UINT64_C(1'000'000),
    UINT64_C(10'000'000),
    UINT64_C(100'000'000),
    UINT64_C(1'000'000'000),
    UINT64_C(10'000'000'000),
    UINT64_C(100'000'000'000),
    UINT64_C(1'000'000'000'000),
    UINT64_C(10'000'000'000'000),
    UINT64_C(100'000'000'000'000),
    UINT64_C(1'000'000'000'000'000),
    UINT64_C(10'000'000'000'000'000),
    UINT64_C(100'000'000'000'000'000),
    UINT64_C(1'000'000'000'000'000'000),
    UINT64_C(10'000'000'000'000'000'000),
  };
}  // namespace tbl

namespace impl {

  // Based on Moller and Granlund
  // https://gmplib.org/~tege/division-paper.pdf

  class fast_divq_t {
  public:
    fast_divq_t(uint64_t d) {
      if (d == 0)
        throw std::out_of_range("divisor cannot be 0");

      uint64_t shift = std::countl_zero(d);
      uint64_t dn    = d << shift;

      // v = ((beta^2 - 1) / dn) - beta
      // Since dn is normalized, (2^64 - 1) / dn is always 1.
      // Hence, we can always subtract off dn and get the 2nd digit, which will
      // be v.
      uint64_t v =
        intr::_divq(0xFFFF'FFFF'FFFF'FFFF - dn, 0xFFFF'FFFF'FFFF'FFFF, dn).a;

      m_d = dn;
      m_v = v;
      m_k = shift;
    }

    friend uint64_x2 fast_divq(
      uint64_t u1, uint64_t u0, const fast_divq_t& div) {
      assert(u1 < (div.m_d >> div.m_k));

      // shift (u1, u0) left to match the normalized quotient
      if (div.m_k > 0) {
        u1 = (u1 << div.m_k) | (u0 >> (64 - div.m_k));
        u0 = u0 << div.m_k;
      }

      // Division algorithm for normalized divisor
      auto [q1, q0] = intr::_mulq(u1, div.m_v);
      q1            = q1 + q0 + 1;
      q0            = q0 + u0;

      uint64_t r = u0 - q1 * div.m_d;
      if (r > q0 && (r + div.m_d) < div.m_d) {
        q1 -= 1;
        r += div.m_d;
      }

      // shift r right to match the non-normalized divisor
      return {q1, r >> div.m_k};
    }

  private:
    // The normalized divisor.
    uint64_t m_d;
    // The magic constant corresponding to m_d.
    uint64_t m_v;
    // The shift needed to go from the actual divisor to m_d.
    uint64_t m_k;
  };

  // Add vector with word. Returns the carry digit.
  uint64_t add_vx(std::span<uint64_t> a, uint64_t b) {
    uint64_t* ptr = &a[0];
    uint64_t* end = &a[a.size()];

    uint64_t cf = __builtin_uaddl_overflow(*ptr, b, ptr);
    ptr++;
    while (cf && ptr != end) {
      *ptr = __builtin_addcl(*ptr, 0, cf, &cf);
      ptr++;
    }

    return cf;
  }

  // Add vector with vector.
  uint64_t add_vv(std::span<uint64_t> a, std::span<const uint64_t> b) {
    assert(a.size() >= b.size());

    uint64_t* ptr_a       = &a[0];
    const uint64_t* ptr_b = &b[0];

    uint64_t* end_a       = &a[a.size()];
    const uint64_t* end_b = &b[b.size()];

    uint64_t cf = false;
    while (ptr_a != end_a) {
      uint64_t wa = *ptr_a;
      uint64_t wb = (ptr_b == end_b) ? 0 : *ptr_b;

      *ptr_a = __builtin_addcl(wa, wb, cf, &cf);

      ptr_a++;
      if (ptr_b != end_b)
        ptr_b++;
    }

    return cf;
  }

  // Subtract vector with vector. Returns 1 if negative; a will contain the
  // two's complement of the answer.
  uint64_t sub_vv(std::span<uint64_t> a, std::span<const uint64_t> b) {
    assert(a.size() >= b.size());

    uint64_t* ptr_a       = &a[0];
    const uint64_t* ptr_b = &b[0];

    uint64_t* end_a       = &a[a.size()];
    const uint64_t* end_b = &b[b.size()];

    uint64_t cf = false;
    while (ptr_a != end_a) {
      uint64_t wa = *ptr_a;
      uint64_t wb = (ptr_b == end_b) ? 0 : *ptr_b;

      *ptr_a = __builtin_subcl(wa, wb, cf, &cf);

      ptr_a++;
      if (ptr_b != end_b)
        ptr_b++;
    }

    return cf;
  }

  // Two's complement of vector.
  void two_comp_v(std::span<uint64_t> a) {
    for (uint64_t& word : a) {
      word = ~word;
    }
    add_vx(a, 1);
  }

  // Multiply vector with word. Returns the carry digit.
  uint64_t mul_vx(std::span<uint64_t> a, uint64_t b) {
    uint64_t* ptr = &a[0];
    uint64_t* end = &a[a.size()];

    uint64_t carry = 0;
    while (ptr != end) {
      auto [hi, lo] = intr::_mulq(*ptr, b);
      *ptr          = lo + carry;
      carry         = hi;
      ptr++;
    }

    return carry;
  }

  // Divide vector by word. Returns the remainder.
  uint64_t div_simple_vx(std::span<uint64_t> a, uint64_t b) {
    uint64_t* ptr = &a[a.size() - 1];
    uint64_t* end = &a[0] - 1;

    uint64_t crem = 0;
    while (ptr != end) {
      auto [quot, rem] = intr::_divq(crem, *ptr, b);
      *ptr             = quot;
      crem             = rem;
      ptr--;
    }
    return crem;
  }

  uint64_t div_precomp_vx(std::span<uint64_t> a, uint64_t b) {
    uint64_t* ptr = &a[a.size() - 1];
    uint64_t* end = &a[0] - 1;

    fast_divq_t div {b};

    uint64_t crem = 0;
    while (ptr != end) {
      auto [quot, rem] = fast_divq(crem, *ptr, div);
      *ptr             = quot;
      crem             = rem;
      ptr--;
    }
    return crem;
  }

  uint64_t div_vx(std::span<uint64_t> a, uint64_t b) {
    if (a.size() < 5) {
      return div_simple_vx(a, b);
    }
    else {
      return div_precomp_vx(a, b);
    }
  }
}  // namespace impl

class svint {
public:
  svint() : m_sign(false), m_words {0} {}

  svint(const svint& rhs) : m_sign(rhs.m_sign), m_words(rhs.m_words) {}
  svint& operator=(const svint& rhs) {
    m_sign  = rhs.m_sign;
    m_words = rhs.m_words;
    return *this;
  }

  svint(svint&& rhs) : m_sign(rhs.m_sign), m_words(rhs.m_words) {}
  svint& operator=(svint&& rhs) {
    m_sign  = rhs.m_sign;
    m_words = rhs.m_words;
    return *this;
  }

  svint(int64_t value) :
    m_sign(value < 0), m_words {(uint64_t) std::abs(value)} {}
  svint(uint64_t value) : m_sign(false), m_words {value} {}

  void reserve(size_t size) { m_words.reserve(size); }

  void set_zero() {
    m_sign = 0;

    m_words.clear();
    m_words.push_back(0);
  }

  bool is_zero() { return m_words.size() == 1 && m_words[0] == 0; }

  void print_debug() {
    printf("{ sign: %c, words: [", m_sign ? '-' : '+');
    size_t i = 0;
    if (i < m_words.size()) {
      printf("%016lX", m_words[i]);
      i++;
    }
    while (i < m_words.size()) {
      printf(", %016lX", m_words[i]);
      i++;
    }
    printf("] }\n");
  }

  svint& operator+=(uint64_t rhs) {
    uint64_t carry = impl::add_vx(m_words, rhs);
    if (carry != 0)
      m_words.push_back(carry);

    return *this;
  }

  svint& operator+=(const svint& rhs) {
    if (m_words.size() < rhs.m_words.size())
      m_words.resize(rhs.m_words.size());

    if (m_sign ^ rhs.m_sign) {
      uint64_t borrow = impl::sub_vv(m_words, rhs.m_words);
      if (borrow) {
        impl::two_comp_v(m_words);
        m_sign = !m_sign;
      }
    }
    else {
      uint64_t carry = impl::add_vv(m_words, rhs.m_words);
      if (carry != 0)
        m_words.push_back(carry);
    }
    fix_sign();

    return *this;
  }

  svint& operator*=(uint64_t rhs) {
    uint64_t carry = impl::mul_vx(m_words, rhs);
    if (carry != 0)
      m_words.push_back(carry);

    return *this;
  }

  uint64_t set_divmod(uint64_t rhs) {
    uint64_t rem = impl::div_vx(m_words, rhs);
    while (m_words.size() > 1 && m_words.back() == 0)
      m_words.pop_back();
    return rem;
  }

  template <class T>
  friend bool operator==(const svint& a, T b)
    requires std::is_unsigned_v<T>
  {
    return !a.m_sign && a.m_words.size() == 1 && a.m_words[0] == b;
  }

  template <class T>
  friend bool operator==(const svint& a, T b)
    requires std::is_signed_v<T>
  {
    return (a.m_sign == (b < 0)) && a.m_words.size() == 1 &&
      a.m_words[0] == std::abs(b);
  }

  friend std::from_chars_result from_chars(
    const char* data, const char* end, svint& value) {
    constexpr uint64_t ONE_E19 = UINT64_C(10'000'000'000'000'000);

    auto first = data;

    value.set_zero();
    if (*data == '-') {
      ++data;
      value.m_sign = true;
    }

    while (end - data >= 19) {
      uint64_t next  = 0;
      auto [ptr, ec] = std::from_chars(data, data + 19, next);
      if (ec != std::errc {}) {
        return std::from_chars_result {.ptr = first, .ec = ec};
      }
      value *= ONE_E19;
      value += next;
    }
    if (end - data > 0) {
      uint64_t last  = 0;
      auto [ptr, ec] = std::from_chars(data, end, last);
      if (ec != std::errc {}) {
        return std::from_chars_result {.ptr = first, .ec = ec};
      }
      value *= tbl::pow10[end - data];
      value += last;
    }

    return std::from_chars_result {.ptr = data, .ec = std::errc()};
  }

  friend std::to_chars_result to_chars(char* data, char* end, svint& value) {
    constexpr uint64_t ONE_E19 = UINT64_C(10'000'000'000'000'000'000);

    static std::vector<uint64_t> cbuf;
    cbuf.reserve(value.m_words.size() * 3 / 2);

    if (data == end) {
      return std::to_chars_result {
        .ptr = end, .ec = std::errc::result_out_of_range};
    }

    if (value.is_zero()) {
      *data++ = '0';
      return std::to_chars_result {.ptr = data, .ec = std::errc()};
    }

    cbuf.clear();
    while (!value.is_zero()) {
      auto rem = value.set_divmod(ONE_E19);
      cbuf.push_back(rem);
    }

    if (value.m_sign) {
      value.m_sign = false;
      *data++      = '-';
    }

    if (data == end) {
      return std::to_chars_result {
        .ptr = end, .ec = std::errc::result_out_of_range};
    }

    for (auto it = cbuf.rbegin(); it != cbuf.rend(); it++) {
      auto [ptr, ec] = std::to_chars(data, end, *it);
      if (ec != std::errc()) {
        return std::to_chars_result {
          .ptr = end, .ec = std::errc::result_out_of_range};
      }
      data = ptr;
    }

    return std::to_chars_result {.ptr = data, .ec = std::errc()};
  }

private:
  void fix_sign() {
    if (is_zero())
      m_sign = false;
  }

  bool m_sign;
  std::vector<uint64_t> m_words;
};

constexpr size_t SB_LEN = 200'100;
constexpr size_t WD_LEN = 6000;

char ibuf[SB_LEN] = {};
char obuf[SB_LEN] = {};

int main() {
  uint32_t count = 0;
  scanf("%u\n", &count);

  svint a;
  svint b;

  a.reserve(WD_LEN);
  b.reserve(WD_LEN);

  for (uint32_t i = 0; i < count; i++) {
    fgets(ibuf, SB_LEN, stdin);
    // find needed lengths
    size_t len   = strlen(ibuf);
    auto end_pos = ibuf + len - 1;
    auto sp_pos  = (char*) memchr(ibuf, ' ', len);
    auto past_sp = sp_pos + 1;
    assert(sp_pos != NULL);

    // trim leading whitespace
    while (isspace(*past_sp))
      past_sp++;

    // trim trailing whitespace
    while (isspace(*end_pos))
      end_pos--;
    end_pos++;

    auto [ptr1, ec1] = from_chars(ibuf, sp_pos, a);
    auto [ptr2, ec2] = from_chars(past_sp, end_pos, b);
    assert(ec1 == std::errc() && ec2 == std::errc());
    // a.print_debug();
    // b.print_debug();

    // special case the one thing that broke

    auto a_copy = a;
    auto b_copy = b;
    a += b;
    // 2474802
    // 2 08 474 2
    // a.print_debug();

    auto [ptr, ec] = to_chars(obuf, obuf + SB_LEN, a);
    assert(ec == std::errc());
    *ptr = '\0';
    printf("%s\n", obuf);
  }
}
