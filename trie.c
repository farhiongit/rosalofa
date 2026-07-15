#include "rosalind.h"

struct node {
  wchar_t value;
  size_t id;
  size_t nb_children;
  struct node *child;
};

static void
node_free (struct node *n) {
  for (size_t i = 0; i < n->nb_children; i++)
    node_free (&n->child[i]);
  free (n->child);
}

static int
node_fprint (struct node *n, FILE *f) {
  int ret = 0;
  for (size_t i = 0; i < n->nb_children; i++) {
    ret += fprintf (f, "%zu %zu %lc\n", n->id + 1, n->child[i].id + 1, (wint_t)n->child[i].value);
    ret += node_fprint (&n->child[i], f);
  }
  return ret;
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  struct node *root = calloc (1, sizeof (*root));
  wchar_t *line = 0;
  size_t id = 0;
  for (struct node *n = root; fgetwcs (&line, 0, stdin); n = root)
    for (wchar_t *p = line; *p && *p != L'\n'; p++) {
      size_t i;
      for (i = 0; i < n->nb_children && *p != n->child[i].value; i++)
        ;
      if (i == n->nb_children) {
        n->child = realloc (n->child, (++n->nb_children) * sizeof (*n->child));
        n->child[n->nb_children - 1] = (struct node){ .value = *p, .id = ++id };
      }
      n = &n->child[i];
    }
  free (line);

  node_fprint (root, stdout);
  node_free (root);
  free (root);
}
