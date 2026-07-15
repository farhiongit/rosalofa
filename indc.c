#include "rosalind.h"

static double
lcomb (unsigned long n, unsigned long p) {
  assert (p <= n);
  if (2 * p > n)
    p = n - p;
  double C = 0.;
  for (unsigned long i = 0; i < p; i++)
    C += log10 ((double)(n - i)) - log10 ((double)(i + 1));
  return C;
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *line = 0;
  assert (fgetwcs (&line, 0, stdin));
  size_t n = wcstoul (line, 0, 0);
  fprintf (stderr, "%zu\n", n);
  free (line);

  for (size_t i = 0; i <= 2 * n; i++)
    fprintf (stderr, "%g ", /*10 * log10 (0.5) + */ lcomb (2 * n, i));
}
