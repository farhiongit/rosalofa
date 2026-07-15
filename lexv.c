#include "rosalind.h"

static void
setletter (char *word, size_t pos, char *letters, size_t nbletters) {
  if (word[pos])
    for (size_t i = 0; i < nbletters; i++) {
      if (letters[i] != ' ') {
        word[pos] = letters[i];
        setletter (word, pos + 1, letters, nbletters);
      } else if (pos) {
        word[pos] = 0;
        fprintf (stdout, "%s\n", word);
        word[pos] = '*';
      }
    }
  else if (pos)
    fprintf (stdout, "%s\n", word);
}

int
main (void) {
  char wcs[100];

  assert (fgets (wcs, 100, stdin));
  char *token;
  char *tokens = malloc (sizeof (*tokens));
  size_t nbtokens = 1;
  tokens[0] = ' ';
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
