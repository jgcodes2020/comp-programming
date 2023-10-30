#include <bits/stdc++.h>
#include <iterator>
#include <memory>
using namespace std;

namespace sort {
  template <class I>
  void print_range(I begin, I end) {
    if (begin == end)
      return;
    std::cout << '[' << *begin++;
    while (begin != end) {
      std::cout << ", " << *begin++;
    }
    std::cout << "]\n";
  }

  template <class I>
  I hoare_partition(I begin, I end) {
    I i = begin - 1, j = end + 1;
    I p = begin;

    while (true) {
      do
        i++;
      while (*i < *p);
      do
        j--;
      while (*j > *p);

      if (i >= j)
        return j;
      swap(*i, *j);
    }
  }

  template <class I>
  void quicksort(I begin, I end) {
    if (begin >= end)
      return;

    I pivot = hoare_partition(begin, end);

    quicksort(begin, pivot);
    quicksort(pivot + 1, end);
  }
}  // namespace sort
int main() {
  size_t size;
  cin >> size;

  auto buffer = std::make_unique<int[]>(size);
  for (size_t i = 0; i < size; i++) {
    cin >> buffer[i];
  }
  cout << '\n';

  sort::quicksort(&buffer[0], &buffer[size - 1]);
  for (size_t i = 0; i < size; i++) {
    cout << buffer[i] << '\n';
  }
}