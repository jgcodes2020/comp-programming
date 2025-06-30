#include <bits/stdc++.h>
#include <immintrin.h>

namespace views = std::ranges::views;

namespace intr {
  struct uint64_x2 {
    uint64_t a, b;
  };

  struct uint64_check {
    uint64_t value;
    bool valid;
  };

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
    const __m128i STAGE2_MASK =
      _mm_set_epi16(1, 100, 1, 100, 1, 100, 1, 100);
    const __m128i STAGE3_MASK =
      _mm_set_epi16(0, 0, 0, 0, 1, 10'000, 1, 10'000);

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
  inline size_t ilog2(uint64_t x) {
    return (x == 0) ? 0 : 64 - __builtin_clzl(x);
  }

  uint64_t ilog10(uint64_t x) {
    size_t guess = tbl::log10_approx_table[ilog2(x)];
    return guess + (x >= tbl::pow10[guess]);
  }

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
  uint64_t div_vx(std::span<uint64_t> a, uint64_t b) {
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

  friend std::from_chars_result from_chars(
    const char* data, const char* end, svint& value) {
    constexpr uint64_t ONE_E16 = UINT64_C(10'000'000'000'000'000);

    auto first = data;

    value.set_zero();
    if (*data == '-') {
      ++data;
      value.m_sign = true;
    }

    while (end - data >= 16) {
      auto [next, valid] = intr::sse42_parse16(data);
      if (!valid) {
        return std::from_chars_result {
          .ptr = first, .ec = std::errc::invalid_argument};
      }
      value *= ONE_E16;
      value += next;

      data += 16;
    }
    if (end - data > 0) {
      uint64_t last = 0;
      auto [ptr, ec] = std::from_chars(data, end, last);
      if (ec != std::errc {}) {
        return std::from_chars_result {
          .ptr = first, .ec = ec
        };
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

    // trim trailing whitespace
    while (isspace(*end_pos))
      end_pos--;
    end_pos++;

    auto [ptr1, ec1] = from_chars(ibuf, sp_pos, a);
    auto [ptr2, ec2] = from_chars(sp_pos + 1, end_pos, b);
    assert(ec1 == std::errc() && ec2 == std::errc());
    // a.print_debug();
    // b.print_debug();

    a += b;
    // a.print_debug();

    auto [ptr, ec] = to_chars(obuf, obuf + SB_LEN, a);
    assert(ec == std::errc());
    *ptr = '\0';
    printf("%s\n", obuf);
  }
}
