#include "rosalind.h"
// #define TRIE

static const char EOL[] = "\r\n";

static void
print_character (size_t character_length, int *character) {
  size_t sum = 0;
  for (size_t i = 0; i < character_length; i++)
    sum += character[i] ? 1 : 0;
  if (1 < sum && sum < character_length - 1) {
    // The character table does not encode trivial characters.
    for (size_t i = 0; i < character_length; i++)
      fprintf (stdout, "%i", character[i] == character[0]); // Starts with 1, to make it comparable with the SHORT version.
    fprintf (stdout, "%s", EOL);                            // LF (not CR-LF)
  }
}

#ifdef TRIE
static void
set_character (struct trie_node *const n, int v, int *character) {
  fprintf (stderr, "%lc [%zu", (wint_t)trie_letter (n), trie_id (n));
  if (trie_is_termination (n))
    fprintf (stderr, ";%zu=%i", *trie_tag (n), character[*trie_tag (n)] = v);
  fprintf (stderr, "]\n");
  for (size_t i = 0; i < trie_nb_children (n); i++)
    set_character (trie_child (n, i), v, character);
}

static void
print_characters (struct trie_node *explore_from, struct trie_node *const root, const size_t character_length, int *const character) {
  size_t first_child = 0;
  if (explore_from == root                    /* If the root... */
      && trie_nb_children (explore_from) == 2 /* has excatly two children... */
      && !trie_is_termination (explore_from) /* and is not a taxon, ... */)
    first_child = 1; /* then splitting with each child is redondant : the first child won't be processed. */
  for (size_t i = 0; i < trie_nb_children (explore_from); i++) {
    struct trie_node *child;
    for (child = trie_child (explore_from, i); trie_nb_children (child) == 1 && !trie_is_termination (child); child = trie_child (child, 0))
      ;
    if (first_child <= i /* see above */
                         //&& trie_is_internal (child) /* The character table does not encode trivial characters. */
    ) {
      fprintf (stderr, "#%lc [%zu]:", (wint_t)trie_letter (child), trie_id (child));
      // set_character (root, 0, character);
      for (size_t j = 0; j < character_length; j++)
        character[j] = 0;
      set_character (child, 1, character);
      print_character (character_length, character);
    }
    print_characters (child, root, character_length, character);
  }
}
#endif

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *line = 0;
  /*
ATCG
GAAG
AAAG
AACG
GACG
ATCC
  */
#ifndef TRIE
  // There are at most two possible choices for the symbol at each position of the strings.
  // For a given SNP (symbol at a single nucleotide position), divide taxa into two sets depending on which of two bases is present at the nucleotide, thus defining the split of a character.
  // A character table for which each character encodes the symbol choice at a single position of the strings.
  // Split of a character : at a single nucleotide position, which of two bases is present at the nucleotide.
  // The character table does not encode trivial characters.
  wchar_t **taxa = 0;
  size_t nb_taxa = 0;
  while (fgetwcs (&line, 0, stdin)) {
    taxa = realloc (taxa, (++nb_taxa) * sizeof (*taxa));
    taxa[nb_taxa - 1] = wcsdup (line);
  }

  int *character = malloc (nb_taxa * sizeof (*character));
  for (size_t j = 0; j < wcslen (taxa[0]); j++) {
    for (size_t i = 0; i < nb_taxa; i++)
      if (j < wcslen (taxa[i]) && iswprint ((wint_t)taxa[i][j]) && iswprint ((wint_t)taxa[0][j]))
        // There are at most two possible choices for the symbol at each position of the strings.
        character[i] = (taxa[i][j] == taxa[0][j]);
      else
        character[i] = 0;
    print_character (nb_taxa, character);
  }
  free (character);
  for (size_t i = 0; i < nb_taxa; i++)
    free (taxa[i]);
  free (taxa);
#else
  struct trie_node *root = trie_init ();
  assert (root);
  for (size_t i = 0; fgetwcs (&line, 0, stdin); i++) {
    struct trie_node *taxon = trie_add (root, line, 1);
    assert (taxon); // Not duplicated.
    *trie_tag (taxon) = i;
  }
  assert (trie_nb_children (root)); // There is a tree.
  trie_fprint (root, stderr);

  const size_t character_length = trie_nb_terminations (root); // Number of taxa.
  int *character = malloc (character_length * sizeof (*character));
  // A character table for which each nontrivial character encodes the symbol choice at a single position of the strings ?
  print_characters (root, root, character_length, character);
  free (character);

  trie_free (root);
#endif
  free (line);
}
