#include "rosalind.h"

int
main (void) {
  wchar_t *line = 0;

  fgetwcs (&line, 0, stdin);
  wchar_t *tok;
  wchar_t *ptr = line;
  tok = wcstok (0, L" \t\n", &ptr);
  size_t n = wcstoul (tok, 0, 0);
  tok = wcstok (0, L" \t\n", &ptr);
  size_t k = wcstoul (tok, 0, 0);
  free (line);
  size_t M = 1000000;
  size_t p = 1;
  for (size_t i = n; i > n - k; i--)
    p = (p * i) % M;
  fprintf (stdout, "%zu", p);
}
