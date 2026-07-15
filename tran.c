#include "rosalind.h"

int
main (void) {
  wchar_t *g1 = 0;
  wchar_t *g2 = 0;
  freadFASTA (0, &g1, stdin);
  freadFASTA (0, &g2, stdin);

  size_t transitions = 0;
  size_t transversions = 0;
  for (size_t i = 0; i < wcslen (g1) && i < wcslen (g2); i++)
    if (g1[i] == g2[i])
      ;
    else if ((g1[i] == L'A' && g2[i] == L'G') || (g1[i] == L'G' && g2[i] == L'A') || (g1[i] == L'C' && g2[i] == L'T') || (g1[i] == L'T' && g2[i] == L'C'))
      transitions++;
    else
      transversions++;
  fprintf (stdout, "%.11f", 1. * (double)transitions / (double)transversions);
  free (g2);
  free (g1);
}
