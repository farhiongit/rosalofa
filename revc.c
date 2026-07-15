#include "rosalind.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static int
f (size_t linenum, size_t toknum, const wchar_t *token, size_t toklen, void *context) {
  (void)linenum;
  (void)toknum;
  (void)context;
  wint_t wc = 0;
  for (size_t i = 0; i < toklen; i++) {
    switch (wc = (wint_t)token[toklen - i - 1]) {
    case L'A':
      wc = L'T';
      break;
    case L'C':
      wc = L'G';
      break;
    case L'G':
      wc = L'C';
      break;
    case L'T':
      wc = L'A';
      break;
    default:
    }
    fprintf (stdout, "%lc", wc);
  }
  fprintf (stdout, "\n");
  return 1;
}

int
main (void) {
  setlocale (LC_ALL, "");
  freadtok (stdin, 0, L"\n", 1, f, 0);
}
