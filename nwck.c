#include "rosalind.h"

// (v1,v2,…,vn)u
// - distance between vi and vj, i != j, is 2 (no parenthesis in between).
// - distance between vi and u is 1 (one closing parenthesis)

// (C, D, (A, B)y)x;
// (A, (D, C)x, B)y;
// ((C,(A,B)y)x)D

// ----------------------------------------
/*TU:
((((A)B,((F),,J))H,C)D)E;
(A,(B,C),D)R;
(C,D, (A,B));
(A, (D,C),B);
(((C,D),A))B;
(((A,B),C))D;
*/
// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (int argc, char **argv) {
  (void)argc;
  setlocale (LC_ALL, "");
  const int nkew = !strcmp ("nkew", strchr (*argv, '/') ? strrchr (*argv, '/') + 1 : *argv);

  wchar_t *sep = 0;
  do {
    // Read from right to left.
    wchar_t *newick = 0;
    if (!fgetwcs (&newick, 0, stdin))
      break;
    if (wcslen (newick) && newick[wcslen (newick) - 1] == L'\n')
      newick[wcslen (newick) - 1] = 0;
    errno = 0;
    struct tree_node *root = tree_read_newick (newick);
    tree_fprint (stderr, root);
    fprintf (stderr, "------\n");

    wchar_t *line = 0;
    assert (fgetwcs (&line, 0, stdin));
    wchar_t *state = line;
    wchar_t *token;

    assert ((token = wcstok (0, L" \t\n", &state)));
    fprintf (stderr, "%ls: ", token);
    const struct tree_node *child1 = tree_get_node_by_name (root, token);
    assert (child1);
    node_fprint_name (stderr, child1);
    node_fprint_ancestors (stderr, child1);
    fprintf (stderr, "\n------\n");

    assert ((token = wcstok (0, L" \t\n", &state)));
    fprintf (stderr, "%ls: ", token);
    const struct tree_node *child2 = tree_get_node_by_name (root, token);
    assert (child2);
    node_fprint_name (stderr, child2);
    node_fprint_ancestors (stderr, child2);
    fprintf (stderr, "\n------\n");

    free (line);

    fprintf (stdout, "%s", sep ? " " : "");
    fprintf (nkew ? stderr : stdout, "%zu", node_get_distance (child1, child2));
    fflush (nkew ? stderr : stdout);
    fprintf (stderr, " ");
    fprintf (!nkew ? stderr : stdout, "%g", node_get_weighted_distance (child1, child2));
    fflush (!nkew ? stderr : stdout);

    tree_free (root);
    free (newick);

    free (sep);
    sep = 0;
  } while (fgetwcs (&sep, 0, stdin));

  return errno ? EXIT_FAILURE : EXIT_SUCCESS;
}
