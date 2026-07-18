#include "trie.h"
#include "rosalind.h"
#include <stdlib.h>

struct trie_node {
  wchar_t letter;
  size_t id;
  size_t nb_children;
  struct trie_node *child; // Array (OK, since there is no link to parent)
  size_t tag;
  int termination;
};

wchar_t
trie_letter (const struct trie_node *const n) { return n->letter; }

size_t *
trie_tag (struct trie_node *const n) { return &n->tag; }

size_t
trie_nb_children (const struct trie_node *const n) { return n->nb_children; }

struct trie_node *
trie_child (const struct trie_node *const n, size_t i) { return &n->child[i]; }

size_t
trie_id (const struct trie_node *const n) { return n->id; }

struct trie_node *
trie_init (void) {
  struct trie_node *const root = calloc (1, sizeof (*root));
  assert (root);
  return root;
}

static void
node_free (struct trie_node *n) {
  for (size_t i = 0; i < n->nb_children; i++)
    node_free (&n->child[i]);
  free (n->child);
}

void
trie_free (struct trie_node *root) {
  node_free (root);
  free (root);
}

int
trie_is_leaf (const struct trie_node *const n) { return n->nb_children == 0; }
int
trie_is_termination (const struct trie_node *const n) { return n->termination != 0; }
int
trie_is_internal (const struct trie_node *const n) { return !trie_is_leaf (n); }

size_t
trie_nb_leaves (const struct trie_node *const n) {
  size_t ret = trie_is_leaf (n) ? 1 : 0;
  for (size_t i = 0; i < n->nb_children; i++)
    ret += trie_nb_leaves (&n->child[i]);
  return ret;
}
size_t
trie_nb_terminations (const struct trie_node *const n) {
  size_t ret = trie_is_termination (n) ? 1 : 0;
  for (size_t i = 0; i < n->nb_children; i++)
    ret += trie_nb_terminations (&n->child[i]);
  return ret;
}
size_t
trie_nb_internals (const struct trie_node *const n) {
  size_t ret = trie_is_internal (n) ? 1 : 0;
  for (size_t i = 0; i < n->nb_children; i++)
    ret += trie_nb_internals (&n->child[i]);
  return ret;
}

struct trie_node *
trie_add (struct trie_node *const root, wchar_t *line, int exclusive) {
  static size_t id = 0;
  struct trie_node *n = root;
  int new_leaf = 0;
  for (wchar_t *p = line; *p && iswprint ((wint_t)*p); p++) {
    size_t i;
    for (i = 0; i < n->nb_children && *p != n->child[i].letter; i++)
      ;
    if (i == n->nb_children) {
      new_leaf = 1;
      assert ((n->child = realloc (n->child, (++n->nb_children) * sizeof (*n->child))));
      n->child[n->nb_children - 1] = (struct trie_node){ .letter = *p, .id = ++id }; // All other fields are set to 0.
    } else if (exclusive && n->child[i].termination)
      return 0;
    n = &n->child[i];
  }
  if (exclusive && !new_leaf)
    return 0;
  if (n->termination) // Duplicated.
    return 0;
  n->termination = 1;
  return n;
}

static int
_trie_fprint (const struct trie_node *const root, FILE *f, size_t indent) {
  int ret = 0;
  for (size_t i = 0; i < indent; i++)
    ret += fprintf (f, ".");
  ret += fprintf (f, "%lc", iswprint ((wint_t)root->letter) ? (wint_t)root->letter : L'*');
  const struct trie_node *n = root;
  for (; n->nb_children == 1 && !n->termination; n = n->child, indent++)
    ret += fprintf (f, "%lc", (wint_t)n->child->letter);
  ret += fprintf (f, " [%zu;%zu]", n->id, n->tag);
  for (size_t i = 0; i < n->nb_children; i++) {
    ret += fprintf (f, "\n");
    ret += _trie_fprint (n->child + i, f, indent + 1);
  }
  return ret;
}

int
trie_fprint (const struct trie_node *const root, FILE *f) {
  return _trie_fprint (root, f, 0) + fprintf (f, "\n");
}

#ifdef TU
static int
node_fprint (const struct trie_node *const n, FILE *f) {
  int ret = 0;
  for (size_t i = 0; i < n->nb_children; i++) {
    ret += fprintf (f, "%zu %zu %lc\n", n->id + 1, n->child[i].id + 1, (wint_t)n->child[i].letter);
    ret += node_fprint (&n->child[i], f);
  }
  return ret;
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  struct trie_node *root = trie_init ();

  wchar_t *line = 0;
  while (fgetwcs (&line, 0, stdin))
    trie_add (root, line, 0);
  free (line);

  node_fprint (root, stdout);
  trie_free (root);
}
#endif
