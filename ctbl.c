#include "rosalind.h"

// Unrooted binary tree :
// - definition: all internal nodes have degree 3 ; leaves have degree 1.
// - property: the number of internal nodes is its number of leaves minus 2 ==> i = l - 2.
// - property: the number of nodes is the its number of edges plus 1 ==> i + l = e + 1
// 2 * l = e + 3.
// 2 * i = e - 1.

// One column for each taxon of the taxa.
//  -> 5 columns: cat, dog, elephnat, mouse, robot

// As many splits as edges (removing an edge being a split).
// Number of characters = number of splits in the tree = number of edges = number of nodes minus 1.
// Number of trivial characters = number of leaves.
// One line for each non-trivial character that differentiate the taxa.
// Number of lines = number of non-trivial characters = e - l = i - 1

// Number of edges between internal nodes = number of edges - number of leaves = e - l = i - 1

// Number of columns = number of taxa = number of names nodes.

// Line i is the ith non-trivial character and is the ith egde between internal characters.
// Column j is the jth taxon (in lexicographic order).
// 1. Find edges between internal characters (how ?).
// 2. For each line i, remove the corresponding edge to get two rooted binary trees, names 0 and 1 (how ?).
// 3. For each column j (jth named node = taxa), set line i to 1 if the the jth named node belongs (how ?) to the tree labeled 1, 0 otherwise.

#define min(a, b) ((a) < (b) ? (a) : (b))
static int
name_cmp (const void *a, const void *b) {
  struct tree_node *na = *(struct tree_node *const *)a;
  struct tree_node *nb = *(struct tree_node *const *)b;
  int cmp = wcsncmp (node_name (na), node_name (nb), min (node_name_length (na), node_name_length (nb)));
  if (cmp)
    return cmp;
  if (node_name_length (nb) > node_name_length (na))
    return -1;
  else if (node_name_length (nb) < node_name_length (na))
    return 1;
  else
    return 0;
}

static size_t
tree_get_named_nodes (const struct tree_node *n, const struct tree_node ***named_nodes) {
  size_t nb_named_nodes = 0;
  *named_nodes = 0;
  if (node_name_length (n)) {
    nb_named_nodes++;
    *named_nodes = realloc (*named_nodes, nb_named_nodes * sizeof (**named_nodes));
    (*named_nodes)[nb_named_nodes - 1] = n;
  }
  for (size_t i = 0; i < node_nb_children (n); i++) {
    const struct tree_node **nn;
    size_t nb = tree_get_named_nodes (node_child (n, i), &nn);
    nb_named_nodes += nb;
    *named_nodes = realloc (*named_nodes, nb_named_nodes * sizeof (**named_nodes));
    for (size_t j = 0; j < nb; j++)
      (*named_nodes)[nb_named_nodes - nb + j] = nn[j];
    free (nn);
  }
  return nb_named_nodes;
}

struct character_table { // Of the current nontrivial character.
  const struct tree_node *taxon;
  int character;
};

static struct character_table *
find_character (const struct tree_node *n, size_t nb_named_nodes, struct character_table *character_table) {
  for (size_t i = 0; i < nb_named_nodes; i++)
    if (character_table[i].taxon == n)
      return &character_table[i];
  return 0;
}

static void
set_character (const struct tree_node *n, int v, size_t nb_named_nodes, struct character_table *character_table) {
  struct character_table *c = find_character (n, nb_named_nodes, character_table);
  if (c)
    c->character = v;
  for (size_t i = 0; i < node_nb_children (n); i++)
    set_character (node_child (n, i), v, nb_named_nodes, character_table);
}

static void
print_characters (const struct tree_node *explore_from, size_t nb_named_nodes, struct character_table *character_table, int found_first_internal) {
  if (node_is_internal (explore_from)) {
    if (found_first_internal) {
      for (size_t i = 0; i < nb_named_nodes; i++)
        character_table[i].character = 0;
      // set_character (tree_root (explore_from), 0, nb_named_nodes, character_table);
      set_character (explore_from, 1, nb_named_nodes, character_table);
      /* display characters;*/
      for (size_t i = 0; i < nb_named_nodes; i++)
        fprintf (stdout, "%i", character_table[i].character);
      fprintf (stdout, "\n");
    }
    found_first_internal = 1;
  }
  for (size_t i = 0; i < node_nb_children (explore_from); i++)
    print_characters (node_child (explore_from, i), nb_named_nodes, character_table, found_first_internal);
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *newick = 0;
  assert (fgetwcs (&newick, 0, stdin));
  fprintf (stderr, "%ls\n", newick);
  struct tree_node *root = tree_read_newick (newick);
  assert (root);
  tree_fprint (stderr, root);

  const struct tree_node **named_nodes = 0;
  size_t nb_named_nodes = 0;
  fprintf (stderr, "%zu:", nb_named_nodes = tree_get_named_nodes (root, &named_nodes));
  qsort (named_nodes, nb_named_nodes, sizeof (*named_nodes), name_cmp);

  struct character_table *character_table = calloc (nb_named_nodes, sizeof (*character_table));
  for (size_t i = 0; i < nb_named_nodes; i++) {
    fprintf (stderr, " ");
    node_fprint_name (stderr, named_nodes[i]);
    character_table[i].taxon = named_nodes[i];
  }
  fprintf (stderr, "\n");
  free (named_nodes);

  print_characters (root, nb_named_nodes, character_table, 0);

  free (character_table);
  tree_free (root);
  free (newick);
}
