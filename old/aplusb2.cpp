#include <bits/stdc++.h>
#include <algorithm>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
using namespace std;

// little-endian, each uint32_t is 9 decimal digits
using uvint = std::vector<uint32_t>;
struct svint {
  uvint mag;
  bool sign;
};

// used for carry
constexpr uint32_t billion = 1000000000;

// reads a uvint from string
void uvint_parse(const char* str, uvint& out) {
  out.clear();
  
}
// reads a uvint from input stream
void uvint_parse(std::istream& in, uvint& out) {
  std::string str;
  in >> str;
  uvint_parse(str.c_str(), out);
}
// writes a uvint to output stream
void uvint_write(std::ostream& out, const uvint& in) {
  ostringstream oss;
  for (auto it = in.rbegin(); it != in.rend(); it++) {
    oss << setw(9) << setfill('0') << *it;
  }
  std::string s = oss.str();
  size_t cutoff = s.find_first_not_of('0');
  if (cutoff == string::npos)
    s = "0";
  else
    s.erase(0, cutoff);
  out << s;
}
// adds two uvints
void uvint_add(const uvint& a, const uvint& b, uvint& out) {
  out.clear();
  bool carry = false;
  auto i = a.begin(), j = b.begin();
  auto k = back_inserter(out);
  bool p = false, q = false;
  while ((p = i != a.end()), (q = j != b.end()), p || q) {
    uint32_t da = p ? *i : 0, db = q ? *j : 0;
    uint32_t sum = da + db + uint32_t(carry);
    carry        = sum >= billion;
    if (carry)
      sum -= billion;
    *k++ = sum;

    if (p)
      i++;
    if (q)
      j++;
  }
}
// subtracts two uvints
void uvint_sub(const uvint& a, const uvint& b, uvint& out) {
  constexpr uint32_t imax = numeric_limits<uint32_t>::max();

  out.clear();
  bool carry = false;
  auto i = a.begin(), j = b.begin();
  auto k = back_inserter(out);
  bool p = false, q = false;
  while ((p = i != a.end()), (q = j != b.end()), p || q) {
    uint32_t da = p ? *i : 0, db = q ? *j : 0;
    da -= carry;
    carry = da < db || da == imax;
    if (carry)
      da += billion;
    *k++ = da - db;

    if (p)
      i++;
    if (q)
      j++;
  }

  if (carry)
    throw std::runtime_error("b > a, unsigned subtract failed");
  // trim "leading" zeros
  while (out.back() == 0 && out.size() > 1)
    out.pop_back();
}
// compares two uvints.
int uvint_cmp(const uvint& a, const uvint& b) {
  
  if (a.size() > b.size())
    return 1;
  else if (a.size() < b.size())
    return -1;
  
  auto i = a.rbegin(), j = b.rbegin();
  while (i != a.rend()) {
    if (*i > *j)
      return 1;
    if (*i < *j)
      return -1;
    
    ++i, ++j;
  }
  return 0;
}

// reads an svint from input stream
void svint_parse(std::istream& in, svint& out) {
  {
    std::istream::sentry sentry(in, false);
    if (!sentry) throw std::runtime_error("Read failed");
    out.sign = in.peek() == '-';
    if (out.sign) in.get();
  }
  
  uvint_parse(in, out.mag);
}
// writes an svint to output stream
void svint_write(std::ostream& out, const svint& in) {
  if (in.sign)
    out << '-';
  uvint_write(out, in.mag);
}
// adds two svints
void svint_add(const svint& a, const svint& b, svint& out) {
  if (a.sign == b.sign) {
    out.sign = a.sign;
    uvint_add(a.mag, b.mag, out.mag);
    return;
  }
  
  int cmp = uvint_cmp(a.mag, b.mag);
  out.sign = (a.sign ^ (cmp < 0)) && (cmp != 0);
  
  if (cmp == 0) {
    out.mag = {0};
  }
  else if (cmp > 0) {
    uvint_sub(a.mag, b.mag, out.mag);
  }
  else {
    uvint_sub(b.mag, a.mag, out.mag);
  }
  
}

int main() {
  size_t n;
  cin >> n;
  
  for (size_t i = 0; i < n; i++)  {
    svint x, y, res;
    svint_parse(cin, x);
    svint_parse(cin, y);
    svint_add(x, y, res);
    
    svint_write(cout, res);
    cout << '\n';
  }
}