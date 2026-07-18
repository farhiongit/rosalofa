#ifndef __TRIE__
#define __TRIE__
#include <stddef.h>
#include <stdio.h>

struct trie_node;
struct trie_node *trie_init (void);
struct trie_node *trie_add (struct trie_node *const root, wchar_t *line, int exclusive); // Returns the leaf node for a new pattern, 0 otherwise.
void trie_free (struct trie_node *root);

wchar_t trie_letter (const struct trie_node *const n);
size_t trie_nb_children (const struct trie_node *const n);
struct trie_node *trie_child (const struct trie_node *const n, size_t i);
size_t trie_id (const struct trie_node *const n);
size_t *trie_tag (struct trie_node *const n);

int trie_is_leaf (const struct trie_node *const n);
int trie_is_termination (const struct trie_node *const n);
int trie_is_internal (const struct trie_node *const n);

size_t trie_nb_leaves (const struct trie_node *const n);
size_t trie_nb_terminations (const struct trie_node *const n);
size_t trie_nb_internals (const struct trie_node *const n);

int trie_fprint (const struct trie_node *const root, FILE *f);
#endif
