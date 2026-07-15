#include "map.h"
#include "rosalind.h"
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int
main (int argc, char **argv) {
  assert (argc >= 3);
  double k = strtod (argv[1], 0);
  double m = strtod (argv[2], 0);
  double n = strtod (argv[3], 0);
  double t = k + m + n;

  fprintf (stdout, "%f\n", (k * (k - 1) + m * k + n * k + k * m + m * (m - 1) * 3 / 4 + n * m * 1 / 2 + k * n + m * n * 1 / 2) / (t * (t - 1)));
}
