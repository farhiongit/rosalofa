#ifndef __NEWICK_H__
#define __NEWICK_H__
#include <stddef.h>
#include <stdio.h>

struct tree_node;

size_t node_name_length (const struct tree_node *n);
const wchar_t *node_name (const struct tree_node *n);
double node_weight (const struct tree_node *n);
const struct tree_node *node_parent (const struct tree_node *n);
const struct tree_node *tree_root (const struct tree_node *n);
size_t node_nb_children (const struct tree_node *n);
const struct tree_node *node_child (const struct tree_node *n, size_t i /* from 0 */);

struct tree_node *tree_read_newick (wchar_t *const newick); // /!\ newick should not be free'd before tree_free is called.
void tree_free (struct tree_node *n);

const struct tree_node *tree_get_node_by_name (const struct tree_node *n, const wchar_t *name);

size_t node_get_depth (const struct tree_node *n);
size_t node_get_distance (const struct tree_node *a, const struct tree_node *b);
double node_get_weighted_depth (const struct tree_node *n);
double node_get_weighted_distance (const struct tree_node *a, const struct tree_node *b);

int node_is_leaf (const struct tree_node *n);
int node_is_internal (const struct tree_node *n);

int node_fprint_name (FILE *f, const struct tree_node *n);
int node_fprint_ancestors (FILE *f, const struct tree_node *n);
int tree_fprint (FILE *f, const struct tree_node *n);
#endif
