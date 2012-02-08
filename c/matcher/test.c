#include <windows.h>
#include <stdio.h>

#include "matcher.h"

static void chop(char * s) {
  int len = strlen(s);

  if (len > 0 && s[len - 1] == '\n') {
    s[len - 1] = 0;
  }
}

int main(int argc, char ** argv) {
  char str[100];
  char pat[100];

  printf("Enter string: ");
  fgets(str, sizeof(str), stdin);
  chop(str);

  while (1) {
    printf("Enter pattern: ");
    fgets(pat, sizeof(pat), stdin);
    chop(pat);

    printf(match_pattern(pat, str) ? "MATCHED!\n" : "no match\n");
  }

  return 1;
}
