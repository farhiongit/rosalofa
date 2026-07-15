#include "rosalind.h"
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int
main (void) {
  wchar_t *line = 0;
  fgetwcs (&line, 0, stdin);
  assert (line && *line);
  wchar_t *startptr = line;
  wchar_t *endptr;
  unsigned long n = wcstoul (startptr, &endptr, 10);
  startptr = endptr;
  unsigned long k = wcstoul (startptr, &endptr, 10);
  free (line);

  unsigned long f, f1, f2;
  f2 = 0;
  f1 = 1;
  f = k;
  for (unsigned long i = 1; i < n; i++) {
    f = k * f2 + f1;
    f2 = f1;
    f1 = f;
  }
  fprintf (stdout, "%lu\n", f);
}
