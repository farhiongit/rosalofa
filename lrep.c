#include "rosalind.h"
//-----------------------------------------------------------
struct rep {
  const struct suffix_node *start;
  const struct suffix_node *end;
  size_t length;
  size_t nb_occurrences;
};

static struct rep
lrep_node_lrep (const struct suffix_node *start, const size_t k, size_t depth) {
  struct rep lrep = { .start = start, .end = start };
  size_t llrep = suffix_node_segment_length (start);
  if (llrep) {
    size_t nb_occurrences = suffix_node_nb_leaves (start);
    if (nb_occurrences >= k) {
      lrep.nb_occurrences = nb_occurrences;
      lrep.length = llrep;
      lrep.end = start;
    }
  }
  for (const void *children = suffix_node_children_iterator (start); children; children = suffix_node_children_iterator_next (children)) {
    const struct suffix_node *n = suffix_node_get_child (children);
    struct rep lrepi = lrep_node_lrep (n, k, depth + 1); // Recursive.
    if (lrepi.nb_occurrences >= k && lrepi.length + llrep > lrep.length) {
      lrep.length = lrepi.length + llrep;
      lrep.nb_occurrences = lrepi.nb_occurrences;
      lrep.end = lrepi.end;
    }
  } // for (struct list *children = start->children; children; children = children->next)
  return lrep;
}
//-----------------------------------------------------------
static void
add_termination (wchar_t **text) {
  // The termination character appears only once, and at the end of the text.
  static const wint_t termchr = L'$';
  wchar_t *endptr;
  if ((endptr = wcschr (*text, termchr))) {
    if (*(endptr + 1))
      fprintf (stderr, "Truncated after '%lc'.\n", termchr);
    *(endptr + 1) = 0;
  } else {
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
  add_termination (&dna);
  fprintf (stderr, "%zu characters.\n", wcslen (dna));

  wchar_t *line = 0;
  assert (fgetwcs (&line, 0, stdin));
  size_t k = wcstoul (line, 0, 10);
  fprintf (stderr, "k = %zu.\n", k);
  assert (k);
  free (line);

  struct suffix_node *root = suffix_node_create (dna);

  fprintf (stderr, "----------\n");
  fprintf (stderr, "%'zu nodes.\n", suffix_node_nb_nodes (root));
  size_t nb_leaves = 0;
  nb_leaves = suffix_node_nb_leaves (root);
  fprintf (stderr, "%'zu leaves.\n", nb_leaves);
  assert (nb_leaves == wcslen (dna));
  // suffix_node_fprint (stderr, root);
  suffix_node_fprint_segments (stderr, root);
  fprintf (stderr, "----------\n");

  struct rep lrep;
  lrep = lrep_node_lrep (root, k, 0);
  (void)lrep;
  assert (lrep.start);
  assert (lrep.end);
  fprintf (stderr, "%'zu-%'zu length=%'zu nb_occurences=%'zu\n", suffix_node_id (lrep.start), suffix_node_id (lrep.end), lrep.length, lrep.nb_occurrences);
  suffix_node_fprint_ancestors (stdout, lrep.end);
  fprintf (stdout, "\n");

  suffix_node_free (root);
  free (dna);
}
