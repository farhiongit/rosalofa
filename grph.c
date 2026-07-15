#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t **dna_name = 0;
  wchar_t **dna = 0;
  size_t nbdna = 0;
  wchar_t *name = 0;
  wchar_t *genetic = 0;
  for (nbdna = 0; freadFASTA (&name, &genetic, stdin); nbdna++) {
    dna_name = realloc (dna_name, (nbdna + 1) * sizeof (*dna_name));
    dna = realloc (dna, (nbdna + 1) * sizeof (*dna));
    dna_name[nbdna] = name;
    dna[nbdna] = genetic;
    fprintf (stderr, ">%ls\n%ls\n", name, genetic);
  }

  static const size_t overlap = 3;
  for (size_t i = 0; i < nbdna; i++)
    for (size_t j = 0; j < nbdna; j++)
      if (i != j && wcslen (dna[i]) >= overlap && wcslen (dna[j]) >= overlap && !wcsncmp (dna[i] + wcslen (dna[i]) - overlap, dna[j], overlap))
        fprintf (stdout, "%ls %ls\n", dna_name[i], dna_name[j]);

  for (size_t i = 0; i < nbdna; i++) {
    free (dna[i]);
    free (dna_name[i]);
  }
  free (dna);
  free (dna_name);
}
