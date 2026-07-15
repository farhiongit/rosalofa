#include "rosalind.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975

static size_t
hamming_distance (wchar_t *sa, wchar_t *sb) {
  size_t hamming_distance = 0;
  for (size_t i = 0; sa[i] && sb[i]; i++)
    if (sa[i] != sb[i])
      hamming_distance++;
  return hamming_distance;
}

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *dna;
  wchar_t **dnas = 0;
  size_t nb_dna = 0;
  while (freadFASTA (0, &dna, stdin)) {
    assert (wcslen (dna));
    dnas = realloc (dnas, (nb_dna + 1) * sizeof (*dnas));
    dnas[nb_dna++] = dna;
  }

  size_t (*distance) (wchar_t *sa, wchar_t *sb) = hamming_distance;
  for (size_t i = 0; i < nb_dna; i++) {
    for (size_t j = 0; j < nb_dna; j++) {
      size_t d = distance (dnas[i], dnas[j]);
      fprintf (stdout, "%s%.5f", j ? " " : "", 1. * (double)d / (double)min (wcslen (dnas[i]), wcslen (dnas[j])));
    }
    fprintf (stdout, "\n");
  }

  for (size_t i = 0; i < nb_dna; i++)
    free (dnas[i]);
  free (dnas);
}
