#include "rosalind.h"
#define min(a, b) ((a) < (b) ? (a) : (b))

struct mrep {
  wchar_t *pattern;
  size_t nb_occurrences;
};

static int
cmp_mrep (const void *a, const void *b) {
  const struct mrep *sa = a;
  const struct mrep *sb = b;
  int ret;
  if (wcslen (sa->pattern) > wcslen (sb->pattern))
    ret = 1;
  else if (wcslen (sa->pattern) < wcslen (sb->pattern))
    ret = -1;
  else
    ret = wcscmp (sa->pattern, sb->pattern);
  return -ret;
}

/*
A maximal repeat of a string s is a repeated substring t of s having two occurrences t1 and t2 such that t1 and t2 cannot be extended by one symbol in either direction in s and still agree.
Not a maximal repeat = not having two occurrences t1 and t2 such that t1 and t2 cannot be extended by one symbol in either direction in s and still agree.
Not a maximal repeat = whatever two occurrences of the repeat, they can be extended by one symbol in either direction in s and still agree.
Not a maximal repeat = whatever two occurrences of the repeat, they can be extended into another common repeat.
Not a maximal repeat = whatever the occurrence of the repeat, it can be extended into another occurrence of the repeat.
Not a maximal repeat = there is another longer repeat in which it is included with an equal or higher number of occurrences.
*/
// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (int argc, char **argv) {
  setlocale (LC_ALL, "");
  assert (argc > 1);
  size_t minlength = strtoul (argv[1], 0, 10); // maximal repeats of s having length at least 20.
  struct mrep *repeats = 0;
  size_t nb_repeats = 0;
  wchar_t *line = 0;
  while (fgetwcs (&line, 0, stdin)) {
    repeats = realloc (repeats, (++nb_repeats) * sizeof (*repeats));
    wchar_t *state = line;
    wchar_t *token;
    assert ((token = wcstok (0, L" \t\n", &state)));
    repeats[nb_repeats - 1].pattern = wcsdup (token);
    assert ((token = wcstok (0, L" \t\n", &state)));
    repeats[nb_repeats - 1].nb_occurrences = wcstoul (token, 0, 10);
  }
  free (line);

  qsort (repeats, nb_repeats, sizeof (*repeats), cmp_mrep);

  for (size_t i = 0; i < nb_repeats; i++)
    for (size_t j = 0; j < i; j++)
      if (wcsstr (repeats[j].pattern, repeats[i].pattern))
        assert (repeats[i].nb_occurrences >= repeats[j].nb_occurrences);

  for (size_t i = 0; i < nb_repeats; i++) {
    for (size_t j = 0; j < i; j++) {
      assert (wcscmp (repeats[j].pattern, repeats[i].pattern));
      if (wcsstr (repeats[j].pattern, repeats[i].pattern))
        repeats[i].nb_occurrences -= min (repeats[i].nb_occurrences, repeats[j].nb_occurrences);
    }
    if (repeats[i].nb_occurrences && wcslen (repeats[i].pattern) >= minlength) {
      fprintf (stdout, "%ls", repeats[i].pattern);
      fflush (stdout);
      fprintf (stderr, " %zu", repeats[i].nb_occurrences);
      fprintf (stdout, "\n");
      fflush (stdout);
    }
  }

  for (size_t i = 0; i < nb_repeats; i++)
    free (repeats[i].pattern);
  free (repeats);
}
