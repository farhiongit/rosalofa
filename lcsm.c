#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t **dna = 0;
  size_t k = 0;
  wchar_t *genetic = 0;
  for (k = 0; freadFASTA (0, &genetic, stdin); k++) {
    dna = realloc (dna, (k + 1) * sizeof (*dna));
    dna[k] = genetic;
    fprintf (stderr, "%ls\n", genetic);
  }

  wchar_t *subwcs = calloc (wcslen (dna[0]) + 1, sizeof (*subwcs));
  wchar_t *lcs = calloc (wcslen (dna[0]) + 1, sizeof (*lcs));
  for (wchar_t *p = dna[0]; p < dna[0] + wcslen (dna[0]); p++)
    for (size_t l = wcslen (lcs) + 1; l <= wcslen (p); l++) {
      wcsncpy (subwcs, p, l);
      *(subwcs + l) = 0;
      fprintf (stderr, " %ls", subwcs);
      for (size_t i = 1; i < k; i++)
        if (!wcsstr (dna[i], subwcs)) {
          l = wcslen (p) + 1;
          break;
        }
      if (l > wcslen (p))
        break;
      wcscpy (lcs, subwcs);
      fprintf (stderr, "*");
    }
  fprintf (stderr, "\n");

  fprintf (stdout, "%ls\n", lcs);
  free (lcs);
  free (subwcs);

  for (size_t i = 0; i < k; i++)
    free (dna[i]);
  free (dna);
}
