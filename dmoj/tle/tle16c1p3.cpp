#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

char io_buf[1024];
char buf2[1024];
#define readline fgets(io_buf, sizeof(io_buf), stdin)
#define io_clear memset(io_buf)

struct compound {
  uint32_t c, h, o;
};

uint64_t gcd(uint64_t a, uint64_t b) {
  uint64_t t;
  while (b != 0) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}
uint64_t gcd(std::initializer_list<uint64_t> items) {
  if (items.size() < 2) abort();
  
  auto it = items.begin();
  auto it2 = it; it2++;
  uint64_t res = ::gcd(*it, *it2);
  ++(++it);
  while (it != items.end()) {
    res = ::gcd(res, *it++);
  }
  return res;
}

// Fractions
#pragma region
struct frac {
  uint64_t n;
  uint64_t d;

  frac() = default;
  constexpr frac(uint64_t x) : n(x), d(1) {}
  constexpr frac(uint64_t n, uint64_t d) : n(n), d(d) {}

  frac reduce() {
    uint64_t gcd = ::gcd(n, d);
    return {n / gcd, d / gcd};
  }
  
  frac reciprocal() {
    return {d, n};
  }
};

#define PF(f) f.n, f.d

frac operator+(frac a, frac b) {
  return {a.n * b.d + b.n * a.d, a.d * b.d};
}
frac operator-(frac a, frac b) {
  return {a.n * b.d - b.n * a.d, a.d * b.d};
}
frac operator*(frac a, frac b) {
  return {a.n * b.n, a.d * b.d};
}
frac operator/(frac a, frac b) {
  // multiply by reciprocal b
  return {a.n * b.d, a.d * b.n};
}
frac& operator+=(frac& a, frac b) {
  return a = a + b;
}
frac& operator-=(frac& a, frac b) {
  return a = a - b;
}
frac& operator*=(frac& a, frac b) {
  return a = a * b;
}
frac& operator/=(frac& a, frac b) {
  return a = a / b;
}
#pragma endregion

compound parse_input() {
  compound res = {0, 0, 0};

  char* begin = io_buf;
  char* end   = io_buf;
  // atom parser
  uint32_t* element;
  begin = end;
  do {
    switch (*begin++) {
      case 'C':
        element = &res.c;
        break;
      case 'H':
        element = &res.h;
        break;
      case 'O':
        element = &res.o;
        break;
      default:
        puts("Invalid element");
        abort();
    }
    int subsc = strtoul(begin, &end, 10);
    if (end == begin) {
      *element += 1;
    }
    else {
      *element += subsc;
    }
    begin = end;
  } while (*end != '\0');
  return res;
}

int main() {
  readline;
  // strip trailing newline
  {
    char* p = io_buf;
    while (*p != '\n') p++;
    *p = '\0';
  }
  
  auto chem = parse_input();
  
  // Equation: a <chem> + b O2 -> c CO2 + d H2O
  // assume a = 1
  frac a = 1;
  // c must equal # of C in <chem> because each CO2 contains 1 C
  frac c = chem.c;
  // d must equal # of H in <chem> / 2 because each H2O contains 2 H
  frac d = chem.h / 2;
  // b: isolate from a + 2b = 2c + d
  // b = (2c + d - $ of O in <chem>) / 2
  frac b = (2 * c + d - chem.o) / 2;
  // reduce
  a = a.reduce();
  b = b.reduce();
  c = c.reduce();
  d = d.reduce();
  
  // Find LCM of denominators
  uint64_t lcm = (a.d * b.d * c.d * d.d) / gcd({a.d, b.d, c.d, d.d});
  // multiply away denominators
  a *= lcm; b *= lcm;
  c *= lcm; d *= lcm;
  // reduce
  a = a.reduce();
  b = b.reduce();
  c = c.reduce();
  d = d.reduce();
  
  if (a.d != 1 || b.d != 1 || c.d != 1 || d.d != 1) {
    fprintf(stderr, "Denominators: %ld, %ld, %ld, %ld\n", a.d, b.d, c.d, d.d);
    abort();
  }
  
  bool check_c = a.n * chem.c == c.n;
  bool check_h = a.n * chem.h == d.n * 2;
  bool check_o = a.n * chem.o + b.n * 2 == c.n * 2 + d.n;
  bool burns = b.n > 0;
  
  if (!(check_c && check_h && check_o && burns)) {
    puts("Impossible");
    return 0;
  }
  
  // Output
  printf("%ld%s + %ldO2 -> %ldCO2 + %ldH2O\n", a.n, io_buf, b.n, c.n, d.n);
}