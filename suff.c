#include "rosalind.h"

static void
lrep_node_add_termination (wchar_t **text) {
  // The termination character appears only once, and at the end of the text.
  static const wint_t termchr = L'$';
  wchar_t *endptr;
  if ((endptr = wcschr (*text, termchr)))
    *(endptr + 1) = 0;
  else {
    size_t l = wcslen (*text);
    assert ((*text = realloc (*text, (l + 2) * sizeof (**text))));
    (*text)[l] = termchr;
    (*text)[l + 1] = 0;
    fprintf (stderr, "Termination '%lc' added.\n", termchr);
  }
  assert (wcschr (*text, termchr) && *(wcschr (*text, termchr) + 1) == 0);
}
//-----------------------------------------------------------
// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *dna = 0;
  assert (fgetwcs (&dna, 0, stdin));
  if (wcslen (dna) > 0 && dna[wcslen (dna) - 1] == L'\n')
    dna[wcslen (dna) - 1] = 0;
  lrep_node_add_termination (&dna);
  fprintf (stderr, "%zu characters.\n", wcslen (dna));

  struct suffix_node *root = suffix_node_create (dna);

  fprintf (stderr, "----------\n");
  fprintf (stderr, "%'zu nodes.\n", suffix_node_nb_nodes (root));
  size_t nb_leaves = 0;
  nb_leaves = suffix_node_nb_leaves (root);
  fprintf (stderr, "%'zu leaves.\n", nb_leaves);
  assert (nb_leaves == wcslen (dna));
  // suffix_node_fprint (stderr, root);
  suffix_node_fprint_segments (stdout, root);
  fprintf (stderr, "----------\n");

  suffix_node_free (root);
  free (dna);
}
