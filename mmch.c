#include "rosalind.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
static uintbig_t
nb_perm (size_t n, size_t p) {
  assert (n >= p);
  uintbig_t nb_perm = 1;
  for (size_t i = n; i > n - p; i--) {
    assert (nb_perm * i > nb_perm);
    nb_perm *= i;
  }
  return nb_perm;
}

int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();

  wchar_t *rna = 0;
  assert (freadFASTA (0, &rna, stdin));
  size_t nA = 0;
  size_t nC = 0;
  size_t nG = 0;
  size_t nU = 0;
  for (wchar_t *p = rna; *p; p++)
    if (*p == L'A')
      nA++;
    else if (*p == L'C')
      nC++;
    else if (*p == L'G')
      nG++;
    else if (*p == L'U')
      nU++;
  free (rna);

  fprintf (stdout, "%U\n", nb_perm (max (nA, nU), min (nA, nU)) * nb_perm (max (nC, nG), min (nC, nG)));
}
