#include "rosalind.h"
// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (int argc, char **argv) {
  setlocale (LC_ALL, "");
  assert (argc > 1);
  int all = atoi (argv[1]);
  wchar_t *dna = 0;
  assert (fgetwcs (&dna, 0, stdin));
  if (wcslen (dna) > 0 && dna[wcslen (dna) - 1] == L'\n')
    dna[wcslen (dna) - 1] = 0;
  helper_add_termination (&dna, L'$');

  struct suffix_node *root = suffix_node_create (dna);
  // suffix_node_fprint_tree (stderr, root);
  suffix_node_fprint_repeats (stdout, root, all);
  suffix_node_free (root);

  free (dna);
}
