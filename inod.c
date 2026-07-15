#include "rosalind.h"

// nbinternals
// nbleaves
// nbnodes = nbedges + 1 = nbinternals + nbleaves
// nbinternals + nbleaves + 1 = nbnodes

// An unrooted binary tree is a rooted binary plus one node linked to the root.
// A binary tree is an agregation of balanced binary trees.
// A binary tree of nb leaves has nb - 1 internal nodes.
// Each of the N aggregated balanced binary trees is a leaf of another binary tree.

// Get an unrooted binary tree : all internal nodes have degree 3.
// Remove an edge between two nodes :
// - those two nodes are disconnected and have degree 2 ;
// - all other internal nodes have degree 3 ;
// - therefore, those two nodes are the root of two disjoint rooted binary trees (only the root has degree 2 ; all other internal nodes have degree 3).
// - All other nodes are left unchanged :
//   - The number of leaves of the two rooted binary trees remains equal to the number of leaves of the unrooted binary tree.
//   - The number of internal nodes of the two rooted binary trees remains equal to the number of internal nodes of the unrooted binary tree.
// As a rooted binary tree of nb leaves has nb - 1 internal nodes, the nb of leaves of two rooted binary is the number of internal nodes plus 2.
// Therefore, the number of internal nodes of the unrooted binary tree is its number of levaves minus 2.

// Other explanation : 3k+n=2⋅((k+n)−1)

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t line[100];
  assert (fgetws (line, 100, stdin));
  size_t nbleaves = wcstoul (line, 0, 0);
  fprintf (stdout, "%zu", nbleaves - 2);
}
