#include "rosalind.h"

static void
setletter (char *word, size_t pos, char *letters, size_t nbletters) {
  if (word[pos]) {
    for (size_t i = 0; i < nbletters; i++) {
      word[pos] = letters[i];
      setletter (word, pos + 1, letters, nbletters);
    }
  } else
    fprintf (stdout, "%s\n", word);
}

int
main (void) {
  char wcs[100];

  assert (fgets (wcs, 100, stdin));
  char *token;
  char *tokens = 0;
  size_t nbtokens = 0;
  for (char *start = wcs; (token = strtok (start, " \t\n")); start = 0, nbtokens++) {
    tokens = realloc (tokens, (nbtokens + 1) * sizeof (*tokens));
    tokens[nbtokens] = *token;
  }

  assert (fgets (wcs, 100, stdin));
  size_t n = strtoul (wcs, 0, 0);

  char *word = calloc ((n + 1), sizeof (*word));
  for (size_t i = 0; i < n; i++)
    word[i] = '*';
  setletter (word, 0, tokens, nbtokens);

  free (tokens);
}
