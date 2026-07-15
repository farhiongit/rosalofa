#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *line;
  line = 0;
  fgetwcs (&line, 0, stdin);
  size_t n = wcstoul (line, 0, 10);
  assert (n);
  free (line);
  fprintf (stderr, "%zu\n", n);

  wchar_t *dna = 0;
  fgetwcs (&dna, 0, stdin);
  for (size_t j = 0; j < wcslen (dna); j++)
    if (dna[j] == L'\n')
      dna[j] = 0;
  fprintf (stderr, "%ls\n", dna);
  assert (dna && *dna && wcslen (dna) <= 10);
  assert (n >= wcslen (dna));

  double *A = 0;
  size_t lA = 0;
  line = 0;
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
    fprintf (stdout, "%s%.3f", i == 0 ? "" : " ", exp10 (log10 ((double)(n + 1 - wcslen (dna))) + B[i]));
  }
  fprintf (stdout, "\n");
  free (B);

  free (A);
  free (dna);
}
