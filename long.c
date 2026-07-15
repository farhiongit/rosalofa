#include "rosalind.h"

static size_t
min (size_t a, size_t b) {
  if (a < b)
    return a;
  else
    return b;
}

static wchar_t *
match (wchar_t *sa, wchar_t *sb) { // sa precedes sb ?
  // unique way to glue together pairs
  // overlap by more than half their length
  wchar_t *ret = 0;
  if (wcslen (sa) && wcslen (sb))
    for (size_t overlap = (min (wcslen (sa), wcslen (sb)) + 1) / 2; overlap <= wcslen (sa) && (ret = wcsstr (sb, sa + wcslen (sa) - overlap)); overlap++)
      if (ret == sb)
        return sa + wcslen (sa) - overlap;

  return 0;
}

struct chain {
  size_t iprec;
  wchar_t *end;
  wchar_t *str;
};

static void
disp (struct chain *chain, size_t i) {
  if (chain[i].iprec)
    disp (chain, chain[i].iprec - 1);
  for (wchar_t *c = chain[i].str; *c && (!chain[i].end || c < chain[i].end); c++)
    fprintf (stdout, "%lc", (wint_t)*c);
}

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t **dnas = 0;
  size_t nb_dnas = 0;
  for (wchar_t *genetic = 0; freadFASTA (0, &genetic, stdin); nb_dnas++) {
    dnas = realloc (dnas, (nb_dnas + 1) * sizeof (*dnas));
    dnas[nb_dnas] = genetic;
  }

  struct chain *chain = calloc (nb_dnas, sizeof (*chain));
  for (size_t i = 0; i < nb_dnas; i++)
    chain[i].str = dnas[i];

  wchar_t *ret = 0;
  for (size_t i = 0; i < nb_dnas; i++)
    for (size_t j = 0; j < nb_dnas; j++)
      if (i != j && (ret = match (dnas[i], dnas[j]))) {
        fprintf (stderr, "%ls %ls\n", dnas[i], dnas[j]);
        chain[j].iprec = i + 1;
        chain[i].end = ret;
      }

  for (size_t i = 0; i < nb_dnas; i++) {
    fprintf (stderr, "%zu ", chain[i].iprec);
    for (wchar_t *c = dnas[i]; *c && (!chain[i].end || c < chain[i].end); c++)
      fprintf (stderr, "%lc", (wint_t)*c);
    fprintf (stderr, "\n");
  }

  for (size_t i = 0; i < nb_dnas; i++)
    if (!chain[i].end)
      disp (chain, i);

  free (chain);
  for (size_t i = 0; i < nb_dnas; i++)
    free (dnas[i]);
  free (dnas);
}
