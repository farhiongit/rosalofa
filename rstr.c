#include "rosalind.h"

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *N_and_x = 0;
  assert (fgetwcs (&N_and_x, 0, stdin) && N_and_x && *N_and_x);

  wchar_t *token;
  wchar_t *nexttok = N_and_x;
  assert ((token = wcstok (0, L" \t\n", &nexttok)));
  wchar_t *ptr;
  unsigned long N = wcstoul (token, &ptr, 10);
  assert (ptr != token);

  assert ((token = wcstok (0, L" \t\n", &nexttok)));
  double x = wcstod (token, &ptr);
  assert (ptr != token);

  free (N_and_x);
  fprintf (stderr, "%lu %f\n", N, x);

  wchar_t *dna = 0;
  assert (fgetwcs (&dna, 0, stdin) && dna && *dna);
  fprintf (stderr, "%ls\n", dna);

  double logCG = log10 (x / 2.);
  double logNotCG = log10 ((1. - x) / 2.);
  double lP = 0;
  for (size_t j = 0; j < wcslen (dna); j++)
    lP += (dna[j] == L'C' || dna[j] == L'G') ? logCG : ((dna[j] == L'A' || dna[j] == L'T' || dna[j] == L'U') ? logNotCG : 0);
  free (dna);

  fprintf (stderr, "%f\n", lP);
  double lNotP = log10 (1. - exp10 (lP));
  fprintf (stderr, "%f\n", lNotP);
  fprintf (stdout, "%.3f\n", 1. - exp10 ((double)N * lNotP));
}
