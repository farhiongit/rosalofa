#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *s = 0;
  if (!freadFASTA (0, &s, stdin))
    return EXIT_FAILURE;

  //                     1                   2
  // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
  // C A G C A T G G T A T C A C A G C A G A G
  // 0 0 0 1 2 0 0 0 0 0 0 1 2 1 2 3 4 5 3 0 0
  size_t *P = calloc (wcslen (s), sizeof (*P));
  size_t currentpos = 0;
  for (size_t i = 1; i < wcslen (s); i++)
    while (1)
      if (s[i] == s[currentpos]) {
        P[i] = ++currentpos;
        break;
      } else if (i && currentpos)
        currentpos = P[currentpos - 1];
      else
        break;

  for (size_t i = 0; i < wcslen (s); i++) {
    fprintf (stdout, "%s", i == 0 ? "" : " ");
    fflush (stdout);
    fprintf (stderr, " %lc ", (wint_t)s[i]);
    fprintf (stdout, "%zu", P[i]);
    fflush (stdout);
  }
  fprintf (stderr, "\n");
  free (P);

  free (s);
}
