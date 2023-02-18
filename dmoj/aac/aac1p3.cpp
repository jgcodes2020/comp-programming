
#include <cstdio>
#include <cstdlib>
using namespace std;

int main() {
  int n, x;
  int p;
  scanf("%d %d", &n, &x);
  
  if ((x % 2) != (n % 2)) {
    puts("-1");
    exit(0);
  }
  
  // number of PAIRs of happy alpacas
  x /= 2;
  
  // haha nice
  for (int i = 0; i < n; i++) {
    if ((i % 2) == 0) {
      p = 420;
    }
    else {
      if (x > 0) {
        p = 420;
        x--;
      }
      else {
        p = 69;
      }
    }
    
    printf("%d", p);
    if (i + 1 < n) {
      putchar(' ');
    }
  }
  puts("");
}