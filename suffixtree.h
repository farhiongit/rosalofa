#ifndef __SUFFIXTREE_H__
#define __SUFFIXTREE_H__

#include <stddef.h>
#include <stdio.h>

struct suffix_node;
struct suffix_node *suffix_node_create (wchar_t *dna);
void suffix_node_free (struct suffix_node *n);

const struct suffix_node *suffix_node_root (const struct suffix_node *n);
const struct suffix_node *suffix_node_parent (const struct suffix_node *n);

size_t suffix_node_id (const struct suffix_node *n);
const wchar_t *suffix_node_segment (const struct suffix_node *n); // Not terminated with 0. Of length suffix_node_segment_length.
size_t suffix_node_segment_length (const struct suffix_node *n);

size_t suffix_node_nb_children (const struct suffix_node *n);
const void *suffix_node_children_iterator (const struct suffix_node *n);
const void *suffix_node_children_iterator_next (const void *it);
const struct suffix_node *suffix_node_get_child (const void *it);

size_t suffix_node_nb_leaves (const struct suffix_node *n);                    // Number of leaves in the tree
size_t suffix_node_nb_nodes (const struct suffix_node *n);                     // Number of nodes (internal and leaves) in the tree
size_t suffix_node_nb_repeats (const struct suffix_node *n, const wchar_t *s); // Number of occurrence of the pattern s in the dna with which the tree was created.

int suffix_node_fprint_tree (FILE *f, const struct suffix_node *n);             // Print tree.
int suffix_node_fprint_node_segment (FILE *f, const struct suffix_node *n);     // Print the segment of a node.
int suffix_node_fprint_ancestors (FILE *f, const struct suffix_node *n);        // Print substring (concatenation of segments) from root to node.
int suffix_node_fprint_segments (FILE *f, const struct suffix_node *n);         // Print all segments.
int suffix_node_fprint_repeats (FILE *f, const struct suffix_node *n, int all); // Print longest (all=0) or all (=1) repeats.

void helper_add_termination (wchar_t **text, wchar_t termchr);
#endif
