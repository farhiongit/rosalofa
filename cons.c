#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *name = 0;
  wchar_t *genetic = 0;
  size_t *profile[5] = { 0 };
  size_t length = 0;
  while (freadFASTA (&name, &genetic, stdin)) {
    fprintf (stderr, ">%ls\n%ls\n", name, genetic);
    if (wcslen (genetic) > length) {
      for (size_t p = 0; p < sizeof (profile) / sizeof (*profile); p++) {
        profile[p] = realloc (profile[p], wcslen (genetic) * sizeof (*profile[p]));
        for (size_t j = length; j < wcslen (genetic); j++)
          profile[p][j] = 0;
      }
      length = wcslen (genetic);
    }
    for (size_t i = 0; i < length; i++)
      profile[genetic[i] == L'A' ? 0 : (genetic[i] == L'C' ? 1 : (genetic[i] == L'G' ? 2 : (genetic[i] == L'T' ? 3 : 4)))][i]++;
  }
  free (name);
  free (genetic);

  wchar_t *consensus = calloc (length, sizeof (*consensus));
  for (size_t i = 0; i < length; i++) {
    size_t vmax = 0;
    size_t pmax = 0;
    for (size_t p = 0; p < sizeof (profile) / sizeof (*profile); p++)
      if (profile[p][i] > vmax) {
        vmax = profile[p][i];
        pmax = p;
      }
    consensus[i] = pmax == 0 ? L'A' : (pmax == 1 ? L'C' : (pmax == 2 ? L'G' : (pmax == 3 ? L'T' : L'U')));
    fprintf (stdout, "%lc", (wint_t)consensus[i]);
  }
  fprintf (stdout, "\n");

  for (size_t p = 0; p < sizeof (profile) / sizeof (*profile); p++) {
    fprintf (stdout, "%lc:", (wint_t)(p == 0 ? L'A' : (p == 1 ? L'C' : (p == 2 ? L'G' : (p == 3 ? L'T' : L'U')))));
    for (size_t j = 0; j < length; j++)
      fprintf (stdout, " %zu", profile[p][j]);
    fprintf (stdout, "\n");
    free (profile[p]);
  }
  free (consensus);
}
