#include <cstddef>
#include <iostream>
#include <string>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  // cin.tie(NULL);

  string word;
  bool value = false;
  while (true) {
    cin >> word;
    if (word == "True") {
      value = !value;
      break;
    }
    else if (word == "False") {
      break;
    }
    else if (word == "not") {
      value = !value;
    }
    else {
      return 69;
    }
  }
  cout << (value ? "True" : "False") << '\n';
}