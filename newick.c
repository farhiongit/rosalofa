#include "newick.h"
//-------------------------------------------------------------------
#include <assert.h>
#include <errno.h>
#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

/*
https://en.wikipedia.org/wiki/Newick_format

tree:
  subtree ';'

subtree:
  node
  '(' list_of_subtrees ')' node

list_of_subtrees:
  subtree
  list_of_subtrees ',' subtree

node:
  name
  name ':' weight

name:
  '' (empty)
  list_of_letters

list_of_letters:
  letter
  list_of_letters letter

letter:
  all signs but '(', ')', ',', ';'

weight:
  decimal number (separator '.')
*/

enum signs {
  sSTART_SIBLINGS = L'(',
  sEND_SIBLINGS = L')',
  sSEP_SIBLINGS = L',',
  sSEP_WEIGHT = L':',
  sEND_TREE = L';',
};

static int
is_reserved (wchar_t l) {
  return (l == sEND_SIBLINGS || l == sSTART_SIBLINGS || l == sSEP_SIBLINGS || l == sSEP_WEIGHT || l == sEND_TREE) ? 1 : 0;
}

static int
is_ignored (wchar_t l) { return iswspace ((wint_t)l) && !is_reserved (l); }
// ----------------------------------------
struct tree_node;
struct tree_node {
  struct tree_node *parent; // Reads top down (the parent is not modified once created).
  double weight;
  struct {
    size_t nb;
    struct tree_node **child;
  } children;
  struct node_name {
    wchar_t *begin, *end_excluded;
  } name;
};
// ----------------------------------------
void
tree_free (struct tree_node *n) {
  for (size_t i = 0; i < n->children.nb; i++)
    tree_free (n->children.child[i]);
  free (n->children.child);
  free (n);
}
// ----------------------------------------
const struct tree_node *
tree_get_node_by_name (const struct tree_node *n, const wchar_t *name) {
  if (n->name.end_excluded > n->name.begin
      && wcslen (name) == (size_t)(n->name.end_excluded - n->name.begin)
      && !wcsncmp (name, n->name.begin, (size_t)(n->name.end_excluded - n->name.begin)))
    return n;

  for (size_t i = 0; i < n->children.nb; i++) {
    const struct tree_node *ret = tree_get_node_by_name (n->children.child[i], name);
    if (ret)
      return ret;
  }
  return 0;
}
// ----------------------------------------
size_t
node_get_depth (const struct tree_node *n) {
  size_t depth = 0;
  for (; n && n->parent; n = n->parent)
    depth++;
  return depth;
}

size_t
node_get_distance (const struct tree_node *a, const struct tree_node *b) {
  size_t da = node_get_depth (a);
  size_t db = node_get_depth (b);
  size_t d = 0;
  for (; b && da + d < db; d++)
    b = b->parent;
  for (; a && db + d < da; d++)
    a = a->parent;
  for (; a && b && a != b; d += 2) {
    a = a->parent;
    b = b->parent;
  }
  return d;
}

double
node_get_weighted_depth (const struct tree_node *n) {
  double depth = 0;
  for (; n; n = n->parent)
    depth += n->weight;
  return depth;
}

double
node_get_weighted_distance (const struct tree_node *a, const struct tree_node *b) {
  size_t da = node_get_depth (a);
  size_t db = node_get_depth (b);
  size_t d = 0;
  double wd = 0;
  for (; b && da + d < db; d++, b = b->parent)
    wd += b->weight;
  for (; a && db + d < da; d++, a = a->parent)
    wd += a->weight;
  for (; a && b && a != b; d += 2, a = a->parent, b = b->parent)
    wd += a->weight + b->weight;
  return wd;
}

int
node_is_leaf (const struct tree_node *n) {
  return n->children.nb <= (n->parent ? 0 : 1);
}

int
node_is_internal (const struct tree_node *n) {
  return !node_is_leaf (n);
}

size_t
node_name_length (const struct tree_node *n) {
  return (size_t)(n->name.end_excluded - n->name.begin);
}

const wchar_t *
node_name (const struct tree_node *n) {
  return n->name.begin;
}

double
node_weight (const struct tree_node *n) {
  return n->weight;
}

const struct tree_node *
node_parent (const struct tree_node *n) {
  return n->parent;
}

const struct tree_node *
tree_root (const struct tree_node *n) {
  for (; n->parent; n = n->parent)
    ;
  return n;
}

size_t
node_nb_children (const struct tree_node *n) {
  return n->children.nb;
}

const struct tree_node *
node_child (const struct tree_node *n, size_t i /* from 0 */) {
  assert (i < n->children.nb);
  return n->children.child[i];
}
//  ----------------------------------------
static wchar_t
READ (wchar_t **ppc) {
  wchar_t *pc = *ppc;
  for (; is_ignored (*pc) && *pc; pc++)
    ;
  *ppc = pc;
  return *pc;
}

static void
read_node_name (wchar_t **cursor, struct node_name *name) {
  name->begin = *cursor;
  for (name->end_excluded = name->begin; *name->end_excluded && !is_reserved (*name->end_excluded); name->end_excluded++)
    ;
  *cursor = name->end_excluded;

  // Trim
  for (; name->begin < name->end_excluded && is_ignored (*(name->begin)); name->begin++)
    ;
  for (; name->begin < name->end_excluded && is_ignored (*(name->end_excluded - 1)); name->end_excluded--)
    ;
}

static struct tree_node *
read_subtree (wchar_t **cursor) {
  struct tree_node *node = calloc (1, sizeof (*node)); // All set to 0.
  if (READ (cursor) == sSTART_SIBLINGS) {
    (*cursor)++;
    for (struct tree_node *subtree = 0; (subtree = read_subtree (cursor)); (*cursor)++) {
      subtree->parent = node; // Reads top down (the parent is not modified once created).
      node->children.child = realloc (node->children.child, (++node->children.nb) * sizeof (*node->children.child));
      node->children.child[node->children.nb - 1] = subtree;
      if (READ (cursor) != sSEP_SIBLINGS)
        break;
    }
    if (READ (cursor) != sEND_SIBLINGS)
      errno = EINVAL; // Invalid format.
    else
      (*cursor)++;
  }
  struct node_name name;
  read_node_name (cursor, &name);
  node->name = name;
  if (READ (cursor) == sSEP_WEIGHT) {
    (*cursor)++;
    wchar_t *end;
    double w = wcstod (*cursor, &end);
    if (end == *cursor)
      errno = EINVAL; // Invalid format.
    else
      *cursor = end;
    node->weight = w;
  }
  return node;
}

// Reads top down (the parent is not modified once created).
struct tree_node *
tree_read_newick (wchar_t *const newick) {
  // Read from left to right.
  wchar_t *cursor = newick;
  struct tree_node *ret = read_subtree (&cursor);
  ret->parent = 0;
  if (READ (&cursor) != sEND_TREE)
    errno = EINVAL; // Invalid format.
  return ret;
}
// ----------------------------------------
int
node_fprint_name (FILE *f, const struct tree_node *n) {
  int ret = 0;
  ret += fprintf (f, "%p[", n);
  for (wchar_t *p = n->name.begin; p < n->name.end_excluded; p++)
    if (iswprint ((wint_t)*p))
      ret += fprintf (f, "%lc", (wint_t)(*p));
  if (n->name.end_excluded == n->name.begin)
    ret += fprintf (f, "?");
  ret += fprintf (f, ":%g]", n->weight);
  ret += node_is_leaf (n) ? fprintf (f, "*") : 0;
  return ret;
}

int
node_fprint_ancestors (FILE *f, const struct tree_node *n) {
  int ret = 0;
  for (struct tree_node *parent = n->parent; parent; parent = parent->parent)
    ret += fprintf (f, " <- ") + node_fprint_name (f, parent);
  return ret;
}

static int
_tree_fprint (FILE *f, const struct tree_node *n, size_t indent) {
  int ret = 0;
  for (size_t i = 0; i < indent; i++)
    ret += fprintf (f, " ");
  ret += node_fprint_name (f, n);
  if (!n->children.nb)
    ret += node_fprint_ancestors (f, n);
  ret += fprintf (f, "\n");
  for (size_t i = 0; i < n->children.nb; i++)
    ret += _tree_fprint (f, n->children.child[i], indent + 1);
  return ret;
}

int
tree_fprint (FILE *f, const struct tree_node *n) {
  return _tree_fprint (f, n, 0);
}
