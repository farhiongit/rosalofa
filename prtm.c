#include "rosalind.h"

/*
A   71.03711
C   103.00919
D   115.02694
E   129.04259
F   147.06841
G   57.02146
H   137.05891
I   113.08406
K   128.09496
L   113.08406
M   131.04049
N   114.04293
P   97.05276
Q   128.05858
R   156.10111
S   87.03203
T   101.04768
V   99.06841
W   186.07931
Y   163.06333
*/

static double m[] = {
  71.03711,  // A
  0,         // B
  103.00919, // C
  115.02694, // D
  129.04259, // E
  147.06841, // F
  57.02146,  // G
  137.05891, // H
  113.08406, // I
  0,         // J
  128.09496, // K
  113.08406, // L
  131.04049, // M
  114.04293, // N
  0,         // O
  97.05276,  // P
  128.05858, // Q
  156.10111, // R
  87.03203,  // S
  101.04768, // T
  0,         // U
  99.06841,  // V
  186.07931, // W
  0,         // X
  163.06333, // Y
  0,         // Z
};

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *protein = 0;
  fgetwcs (&protein, 0, stdin);
  fprintf (stderr, "%ls\n", protein);

  double M = 0;
  for (size_t i = 0; i < wcslen (protein); i++)
    if (L'A' <= protein[i] && protein[i] <= L'Z')
      M += m[protein[i] - L'A'];

  fprintf (stdout, "%.3f\n", M);
  free (protein);
}
