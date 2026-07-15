#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *line = 0;
  fgetwcs (&line, 0, stdin);
  assert (line && *line);

  wchar_t *startptr;
  wchar_t *endptr = line;

  size_t N[6] = { 0 };
  for (size_t i = 0; i < lengthof (N); i++) {
    startptr = endptr;
    N[i] = wcstoull (startptr, &endptr, 10);
    fprintf (stderr, "%zu ", N[i]);
  }
  fprintf (stderr, "\n");
  free (line);

  fprintf (stdout, "%g\n", 2 * ((double)N[0] + (double)N[1] + (double)N[2] + 0.75 * (double)N[3] + 0.5 * (double)N[4]));
}
