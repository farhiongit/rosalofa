#include "suffixtree.h"
#include "rosalind.h"
//-----------------------------------------------------------
struct list {
  void /* struct suffix_node */ *elem;
  struct list *next;
};

struct suffix_node {
  // Node
  size_t id;
  // Upstream edge
  struct {
    wchar_t *begin, *end_excluded;
  } segment;
  struct suffix_node *parent;
  // Downstream edges
  size_t nb_children;
  struct list *children; // List of children.
};

/*
https://en.wikipedia.org/wiki/Suffix_tree

The suffix tree for the string S of length n is defined as a tree such that:
- The tree has exactly n leaves numbered from 1 to n.
- Except for the root, every internal node has at least two children.
- Each edge is labelled with a non-empty substring of S.
- No two edges starting out of a node can have string-labels beginning with the same character.
- The string obtained by concatenating all the string-labels found on the path from the root to leaf i spells out suffix S [i..n], for i from 1 to n.
*/

static void
suffix_node_tree_assert (struct suffix_node *n) {
  assert (n->segment.begin && n->segment.end_excluded);
  assert (n->parent ? n->segment.begin < n->segment.end_excluded : n->segment.begin == n->segment.end_excluded);
  assert (!n->parent || n->nb_children != 1);
  for (struct list *child1 = n->children; child1; child1 = child1->next)
    for (struct list *child2 = n->children; child2; child2 = child2->next)
      assert (child1 == child2 || *((struct suffix_node *)child1->elem)->segment.begin != *((struct suffix_node *)child2->elem)->segment.begin);
  size_t nb_children = 0;
  for (struct list *child = n->children; child; child = child->next, nb_children++) {
    assert (((struct suffix_node *)child->elem)->parent == n);
    suffix_node_tree_assert (child->elem);
  }
  assert (nb_children == n->nb_children);
}

static void
suffix_node_add_suffix (struct suffix_node *root, wchar_t *suffix, size_t *id) {
  wchar_t *dna = root->segment.begin;
  size_t ldna = wcslen (dna);
  wchar_t *suffix_node_pos = root->segment.begin;
  struct suffix_node *n = root;
  for (wchar_t *p = suffix; *p; p++) {
    assert (dna <= suffix_node_pos && suffix_node_pos <= dna + ldna);
    if (suffix_node_pos < n->segment.end_excluded) {
      assert (n != root);
      assert (*suffix_node_pos);
      if (*p == *suffix_node_pos)
        // Continue on the same node.
        suffix_node_pos++;
      else {
        // Split into two new children.
        // Node 1 inherent from the splitted node n.
        struct suffix_node *node = malloc (sizeof (*node));
        *node = (struct suffix_node){ .segment = { .begin = suffix_node_pos, .end_excluded = n->segment.end_excluded }, .id = (*id)++, .parent = n, .nb_children = n->nb_children, .children = n->children };
        // Change parents from n to node 1
        for (struct list *children = n->children; children; children = children->next)
          ((struct suffix_node *)children->elem)->parent = node;
        n->segment.end_excluded = suffix_node_pos;
        n->children = malloc (sizeof (*n->children));
        *(n->children) = (struct list){ .elem = node };
        // Node 2 is a brand new child.
        node = malloc (sizeof (*node));
        *node = (struct suffix_node){ .segment = { .begin = p, .end_excluded = p + 1 }, .id = (*id)++, .parent = n };
        n->children->next = malloc (sizeof (*n->children->next));
        *(n->children->next) = (struct list){ .elem = node };
        n->nb_children = 2;
        // Continue on node 2.
        suffix_node_pos = (n = node)->segment.begin + 1;
      }
    } else if (/* suffix_node_pos == n->segment.end_excluded && */ n != root && n->nb_children == 0) {
      assert (n->segment.begin < n->segment.end_excluded);
      assert (suffix_node_pos == n->segment.end_excluded);
      // Extend the substring of the current node.
      assert (*n->segment.end_excluded);
      n->segment.end_excluded++;
      // Continue on the same node.
      assert (*suffix_node_pos);
      assert (*p == *suffix_node_pos);
      suffix_node_pos++;
    } else {
      // Search for the next edge.
      struct list *child;
      for (child = n->children; child && *p != *(((struct suffix_node *)(child->elem))->segment.begin); child = child->next)
        ;
      if (!child) {
        // Brand new child.
        struct suffix_node *node = malloc (sizeof (*node));
        *node = (struct suffix_node){ .segment = { .begin = p, .end_excluded = p + 1 }, .id = (*id)++, .parent = n };
        child = malloc (sizeof (*child));
        *child = (struct list){ .next = n->children, .elem = node };
        n->children = child;
        n->nb_children++;
      }
      // Continue on the found or new node.
      suffix_node_pos = ((struct suffix_node *)(n = child->elem))->segment.begin + 1;
    }
  } // for (wchar_t *p = suffix; *p; p++)
}

struct suffix_node *
suffix_node_create (wchar_t *dna) {
  struct suffix_node *root = calloc (1, sizeof (*root));
  root->segment.begin = root->segment.end_excluded = dna;
  size_t id = 1;
  for (wchar_t *suffix = dna; *suffix; suffix++)
    suffix_node_add_suffix (root, suffix, &id);
  suffix_node_tree_assert (root);
  assert (suffix_node_nb_leaves (root) == wcslen (dna));
  return root;
}

void
suffix_node_free (struct suffix_node *n) {
  struct list *next;
  for (struct list *children = n->children; children; children = next) {
    suffix_node_free (children->elem);
    next = children->next;
    free (children);
  }
  free (n);
}
//-----------------------------------------------------------
size_t
suffix_node_id (const struct suffix_node *n) { return n->id + 1; }
const wchar_t *
suffix_node_segment (const struct suffix_node *n) { return n->segment.begin; }
size_t
suffix_node_segment_length (const struct suffix_node *n) { return (size_t)(n->segment.end_excluded - n->segment.begin); }
size_t
suffix_node_nb_children (const struct suffix_node *n) { return n->nb_children; }
const void *
suffix_node_children_iterator (const struct suffix_node *n) { return n->children; }
const void *
suffix_node_children_iterator_next (const void *n) { return ((const struct list *)n)->next; }
const struct suffix_node *
suffix_node_get_child (const void *n) { return ((const struct list *)n)->elem; }
const struct suffix_node *
suffix_node_parent (const struct suffix_node *n) { return n->parent; }
const struct suffix_node *
suffix_node_root (const struct suffix_node *n) {
  for (; n->parent; n = n->parent)
    ;
  return n;
}

size_t
suffix_node_nb_leaves (const struct suffix_node *n) {
  if (!n->children)
    return 1;
  size_t nb = 0;
  for (struct list *children = n->children; children; children = children->next)
    nb += suffix_node_nb_leaves (children->elem);
  return nb;
}

size_t
suffix_node_nb_nodes (const struct suffix_node *n) {
  if (!n)
    return 0;
  size_t nb = 1;
  for (struct list *children = n->children; children; children = children->next)
    nb += suffix_node_nb_nodes (children->elem);
  return nb;
}
//-----------------------------------------------------------
size_t
suffix_node_nb_repeats (const struct suffix_node *root, const wchar_t *s) {
  const wchar_t *suffix_node_pos = suffix_node_segment (root);
  const struct suffix_node *n = root;
  for (const wchar_t *p = s; *p; p++) {
    if (n != root && suffix_node_pos < suffix_node_segment (n) + suffix_node_segment_length (n)) {
      if (*p == *suffix_node_pos)
        // Continue on the same node.
        suffix_node_pos++;
      else
        return 0;
    } else {
      const struct list *child;
      for (child = suffix_node_children_iterator (n);
           child && *p != *suffix_node_segment (suffix_node_get_child (child));
           child = suffix_node_children_iterator_next (child))
        ;
      if (!child)
        return 0;
      // Continue on the next node.
      suffix_node_pos = suffix_node_segment (n = suffix_node_get_child (child)) + 1;
    }
  }
  return suffix_node_nb_leaves (n);
}
//-----------------------------------------------------------
int
suffix_node_fprint_node_segment (FILE *f, const struct suffix_node *n) {
  int ret = 0;
  for (wchar_t *p = n->segment.begin; p < n->segment.end_excluded; p++)
    if (iswprint ((wint_t)*p))
      ret += fprintf (f, "%lc", (wint_t)*p);
  return ret;
}

int
suffix_node_fprint_ancestors (FILE *f, const struct suffix_node *n) {
  if (!n)
    return 0;
  int ret = 0;
  ret += suffix_node_fprint_ancestors (f, n->parent);
  ret += suffix_node_fprint_node_segment (f, n);
  return ret;
}

static int
_suffix_node_fprint (FILE *f, const struct suffix_node *n, const size_t indent) {
  int ret = 0;
  for (size_t i = 0; i < indent; i++)
    ret += fprintf (f, i % 5 ? " " : ".");
  fprintf (f, "'");
  ret += suffix_node_fprint_node_segment (f, n);
  ret += fprintf (f, "'[%'zu] ", n->id + 1);
  if (n->nb_children == 0) {
    fprintf (f, "(");
    suffix_node_fprint_ancestors (f, n);
    fprintf (f, ")");
  }
  ret += fprintf (f, "\n");
  for (struct list *children = n->children /* n->nb_children != 1 */; children; children = children->next) {
    assert (((const struct suffix_node *)children->elem)->parent == n);
    ret += _suffix_node_fprint (f, children->elem, indent + 1);
  }
  return ret;
}

int
suffix_node_fprint_tree (FILE *f, const struct suffix_node *n) {
  return _suffix_node_fprint (f, n, 0);
}

int
suffix_node_fprint_segments (FILE *f, const struct suffix_node *n) {
  int ret = 0;
  ret += suffix_node_fprint_node_segment (f, n);
  ret += ret ? fprintf (f, "\n") : 0;
  for (struct list *children = n->children; children; children = children->next)
    ret += suffix_node_fprint_segments (f, children->elem);
  return ret;
}

int
suffix_node_fprint_repeats (FILE *f, const struct suffix_node *start, int all) {
  int longer = 0;
  for (const void *children = suffix_node_children_iterator (start); children; children = suffix_node_children_iterator_next (children))
    longer |= suffix_node_fprint_repeats (f, suffix_node_get_child (children), all);
  if ((!longer || all) && suffix_node_nb_children (start) > 1) {
    longer = 1;
    if (suffix_node_fprint_ancestors (f, start))
      fprintf (f, " %zu\n", suffix_node_nb_leaves (start));
  }
  return longer;
}
//-----------------------------------------------------------
void
helper_add_termination (wchar_t **text, wchar_t termchr) {
  // The termination character appears only once, and at the end of the text.
  wchar_t *endptr;
  if ((endptr = wcschr (*text, termchr))) {
    if (*(endptr + 1))
      fprintf (stderr, "Truncated after first '%lc'.\n", (wint_t)termchr);
    *(endptr + 1) = 0;
  } else {
    size_t l = wcslen (*text);
    assert ((*text = realloc (*text, (l + 2) * sizeof (**text))));
    (*text)[l] = termchr;
    (*text)[l + 1] = 0;
    fprintf (stderr, "Termination '%lc' added.\n", (wint_t)termchr);
  }
  assert (wcschr (*text, termchr) && *(wcschr (*text, termchr) + 1) == 0);
}
