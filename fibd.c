#include "rosalind.h"

#ifdef DEBUG
#define PRINTBI(UBI)                       \
  do {                                     \
    fprint_UBI (stderr, UBI);              \
    fprintf (stderr, " [%i]\n", __LINE__); \
  } while (0)
#else
#define PRINTBI(UBI)
#endif
int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();
  wchar_t *line = 0;
  fgetwcs (&line, 0, stdin);
  assert (line && *line);

  wchar_t *startptr;
  wchar_t *endptr = line;

  startptr = endptr;
  size_t n = wcstoull (startptr, &endptr, 10);
  assert (n);
  startptr = endptr;
  size_t m = wcstoull (startptr, &endptr, 10);
  assert (m);
  startptr = endptr;
  uintbig_t k = wcstoull (startptr, &endptr, 10);
  if (!k)
    k = 1;
  assert (k);
  free (line);

  uintbig_t *F = calloc (m, sizeof (*F)); // F[i] : population with age = i+1, i from 0 to m-1;
  assert (F);
  uintbig_t P = F[0] = 1;
  for (size_t i = 1; i < n; i++) {
    // Give birth to offsprings and die
    uintbig_t B = k * (P - F[0]);
    PRINTBI (B);
    // Add newbies, remove deaths
    P += k * (P - F[0]) - F[m - 1];
    PRINTBI (P);
    // Grow old
    for (size_t j = m; j >= 2; j--)
      F[j - 1] = F[j - 2];
    F[0] = B;
    for (size_t j = 0; j < m; j++)
      PRINTBI (F[j]);
  }
  fprintf (stdout, "%" PRIUINTBIG "\n", P);
  free (F);
}
