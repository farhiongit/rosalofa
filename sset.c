#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t line[10];
  assert (fgetws (line, lengthof (line), stdin));

  unsigned long n = wcstoull (line, 0, 10);
  static const unsigned long m = 1000000;
  unsigned long ns = 1;
  for (unsigned long i = 0; i < n; i++)
    ns = (2 * ns) % m;

  fprintf (stdout, "%lu\n", ns);
}
