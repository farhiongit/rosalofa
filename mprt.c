// while read -r uniprot ; do print ">$uniprot" ; wget -q "https://rest.uniprot.org/uniprotkb/${uniprot%%_*}.fasta" -O - ; done <<EOF | LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975 ./mprt

#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t **prot_name = 0;
  wchar_t **prot = 0;
  size_t nbprot = 0;
  wchar_t *name = 0;
  wchar_t *genetic = 0;
  for (nbprot = 0; fgetwcs (&name, 0, stdin) && freadFASTA (0, &genetic, stdin); nbprot++, name = 0) {
    prot_name = realloc (prot_name, (nbprot + 1) * sizeof (*prot_name));
    prot = realloc (prot, (nbprot + 1) * sizeof (*prot));
    prot_name[nbprot] = name;
    prot[nbprot] = genetic;
    fprintf (stderr, ">%ls%ls\n", name + 1, genetic);
  }

  // wchar_t *Nglycosylation = L"N{P}[ST]{P}";

  for (size_t i = 0; i < nbprot; i++) {
    size_t nbpos = 0;
    size_t *pos = 0;
    for (size_t j = 0; j < wcslen (prot[i]) - 4; j++) {
      if (prot[i][j] == L'N' && prot[i][j + 1] != L'P' && (prot[i][j + 2] == L'S' || prot[i][j + 2] == L'T') && prot[i][j + 3] != L'P') {
        pos = realloc (pos, ++nbpos * sizeof (*pos));
        pos[nbpos - 1] = j + 1;
        fprintf (stderr, "%zu ", j + 1);
      }
    }
    if (nbpos) {
      fprintf (stdout, "%ls", prot_name[i] + 1);
      for (size_t j = 0; j < nbpos; j++)
        fprintf (stdout, "%s%zu", j == 0 ? "" : " ", pos[j]);
      fprintf (stdout, "\n");
    }
    free (pos);
  }

  for (size_t i = 0; i < nbprot; i++) {
    free (prot[i]);
    free (prot_name[i]);
  }
  free (prot);
  free (prot_name);
}
