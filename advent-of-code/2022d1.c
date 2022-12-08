#include <stdio.h>
#include <stdlib.h>

char ibuf[256] = {};
int top3[3] = {};

int main() {
  int total = 0;
  int max_total = 0;
  
  while (!feof(stdin)) {
    fgets(ibuf, sizeof(ibuf), stdin);
    if (ibuf[1] == 0) {
      // end of current elf's total
      printf("Total: %d\n", total);
      if (total > max_total) {
        for (int i = (sizeof(top3) / sizeof(int)); i > 0; i--) {
          top3[i - 1] = top3[i - 2];
        }
        top3[0] = total;
      }
      total = 0;
    }
    else {
      total += (int) strtol(ibuf, NULL, 10);
    }
  }
  
  printf("Maximum value: %d", top3[0] + top3[1] + top3[2]);
}