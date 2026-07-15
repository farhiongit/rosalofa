#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *dna = 0;
  fgetwcs (&dna, 0, stdin);
  assert (dna && *dna);
  fprintf (stderr, "%ls\n", dna);

  double *A = 0;
  size_t lA = 0;
  wchar_t *line = 0;
  fgetwcs (&line, 0, stdin);
  assert (line && *line);
  wchar_t *startptr;
  wchar_t *endptr = line;
  while (1) {
    startptr = endptr;
    double d = wcstod (startptr, &endptr);
    if (endptr == startptr)
      break;
    A = realloc (A, ++lA * sizeof (*A));
    A[lA - 1] = d;
  }
  free (line);
  for (size_t i = 0; i < lA; i++)
    fprintf (stderr, "%s%g ", i == 0 ? "" : " ", A[i]);
  fprintf (stderr, "\n");

  double *B = malloc (lA * sizeof (*B));
  for (size_t i = 0; i < lA; i++) {
    double logCG = log10 (A[i] / 2);
    double logNotCG = log10 ((1 - A[i]) / 2);
    B[i] = 0;
    for (size_t j = 0; j < wcslen (dna); j++)
      B[i] += (dna[j] == L'C' || dna[j] == L'G') ? logCG : ((dna[j] == L'A' || dna[j] == L'T' || dna[j] == L'U') ? logNotCG : 0);
    fprintf (stdout, "%s%.3f", i == 0 ? "" : " ", B[i]);
  }
  fprintf (stdout, "\n");
  free (B);

  free (A);
  free (dna);
}
