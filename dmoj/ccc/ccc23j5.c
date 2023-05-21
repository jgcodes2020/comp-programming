#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char has_incoming[100][100];
char w[102];

int main() {
  int r, c, wlen, cnt = 0, diff;
  bool flag;

  fgets(w, sizeof(w), stdin);
  wlen = strlen(w) - 1;

  scanf("%d %d", &r, &c);

  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      scanf(" %c", &has_incoming[i][j]);
    }
    has_incoming[i][c] = '\0';
  }
  
  // master loop
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      // R->U/D
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (j + k >= c)
          break;
        // R-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i][j + k] == w[k]);
          break;
        }
        // no matches from here
        if (has_incoming[i][j + k] != w[k])
          break;
        
        diff = wlen - k;
        // R->D
        if (i + diff <= r && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + l][j + k] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // R->U
        if (i + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - l][j + k] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // L->U/D
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (j - k < 0)
          break;
        // L-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i][j - k] == w[k]);
          break;
        }
        // no more matches found from here
        if (has_incoming[i][j - k] != w[k]) {
          break;
        }
        
        diff = wlen - k;
        // L->D
        if (i + diff <= r && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + l][j - k] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // L->U
        if (i + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - l][j - k] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // U/D-only matches checked in R->U/D search
      // so we don't need to do that
      // D->L/R
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (i + k >= r)
          break;
        // D-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i + k][j] == w[k]);
          break;
        }
        // no more matches found from here
        if (has_incoming[i + k][j] != w[k])
          break;
          
        diff = wlen - k;
        // D->R
        if (j + diff <= r && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + k][j + l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // D->L
        if (j + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + k][j - l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // U->L/R
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (i - k < 0)
          break;
        // U-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i - k][j] == w[k]);
          break;
        }
        // no more matches found from here
        if (has_incoming[i - k][j] != w[k])
          break;
          
        diff = wlen - k;
        // U->R
        if (j + diff <= r && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - k][j + l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // U->L
        if (j + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - k][j - l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // DR->UR/DL
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (j + k >= c || i + k >= r)
          break;
        // diagonal-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i + k][j + k] == w[k]);
          break;
        }
        // no matches from here
        if (has_incoming[i + k][j + k] != w[k])
          break;
        diff = wlen - k;
        // DR->DL
        if (i + k + diff <= r && j + k + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + k + l][j + k - l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // DR->UR
        if (i + k + 1 - diff >= 0 && j + k + diff <= c && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + k - l][j + k + l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // DL->UL/DR
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (j - k < 0 || i + k >= r)
          break;
        // diagonal-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i + k][j - k] == w[k]);
          break;
        }
        // no matches from here
        if (has_incoming[i + k][j - k] != w[k])
          break;
        diff = wlen - k;
        // DL->UL
        if (i + k + 1 - diff >= 0 && j - k + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + k - l][j - k - l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // DL->DR
        if (i + k + diff <= c && j - k + diff <= c && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i + k + l][j - k + l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // UR->DR/UL
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (j + k >= r || i - k < 0)
          break;
        // diagonal-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i - k][j + k] == w[k]);
          break;
        }
        // no matches from here
        if (has_incoming[i - k][j + k] != w[k])
          break;
        diff = wlen - k;
        // UR->UL
        if (i - k + 1 - diff >= 0 && j + k + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - k - l][j + k - l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // UR->DR
        if (i - k + diff <= c && j + k + diff <= c && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - k + l][j + k + l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
      // UL->DL/UR
      for (int k = 0; k < wlen; k++) {
        // out of bounds
        if (j - k < 0 || i - k < 0)
          break;
        // diagonal-only match
        if (k + 1 == wlen) {
          cnt += (int) (has_incoming[i - k][j - k] == w[k]);
          break;
        }
        // no matches from here
        if (has_incoming[i - k][j - k] != w[k])
          break;
        diff = wlen - k;
        // UL->DL
        if (i - k + diff <= r && j - k + 1 - diff >= 0 && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - k + l][j - k - l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
        // UL->UR
        if (i - k + 1 - diff >= 0 && j - k + diff <= c && k >= 1) {
          flag = true;
          for (int l = 1; l < diff; l++) {
            if (has_incoming[i - k - l][j - k + l] != w[k + l]) {
              flag = false;
              break;
            }
          }
          cnt += (int) flag;
        }
      }
    }
  }
  
  printf("%d\n", cnt);
  fflush(stdout);
}