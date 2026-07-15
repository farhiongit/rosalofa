#include "rosalind.h"

static wchar_t
complement (wchar_t b) {
  return b == L'A' ? L'T' : (b == L'C' ? L'G' : (b == L'G' ? L'C' : L'A'));
}

static void
rc_free (int ret, void *arg) {
  (void)ret;
  free (*(void **)arg);
}

static wchar_t *
rc (wchar_t *a) {
  static size_t length = 0;
  static wchar_t *ret = 0;
  if (!ret)
    assert (!on_exit (rc_free, &ret));
  size_t len = wcslen (a);
  if (length <= len)
    assert ((ret = realloc (ret, (length = (len + 1)) * sizeof (*ret))));
  for (size_t i = 0; i < len; i++)
    ret[i] = complement (a[len - i - 1]);
  ret[len] = 0;
  return ret;
}

static size_t
hd (wchar_t *a, wchar_t *b) {
  assert (wcslen (a) == wcslen (b));
  size_t ret = 0;
  size_t len = wcslen (a);
  for (size_t i = 0; a[i] && i < len; i++)
    if (a[i] != b[i])
      ret++;
  return ret;
}

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t **sequences = 0;
  size_t nb_sequences = 0;
  wchar_t *sequence = 0;
  while (freadFASTA (0, &sequence, stdin)) {
    sequences = realloc (sequences, (++nb_sequences) * sizeof (*sequences));
    sequences[nb_sequences - 1] = sequence;
  }

  assert (nb_sequences);
  size_t nb_correct = 0;
  size_t nb_incorrect = 0;

  // s was correctly sequenced and appears in the dataset at least twice (possibly as a reverse complement);
  for (size_t i = 0; i < nb_sequences; i++)
    for (size_t j = 0; j < nb_sequences; j++)
      if (i != j && (hd (sequences[i], sequences[j]) == 0 || hd (sequences[i], rc (sequences[j])) == 0)) {
        fprintf (stderr, "%ls==%ls\n", sequences[j], sequences[i]);
        wchar_t *tmp = sequences[i];
        sequences[i] = sequences[nb_correct];
        sequences[nb_correct++] = tmp;
        break;
      }

  // s is incorrect, it appears in the dataset exactly once, and its Hamming distance is 1 with respect to exactly one correct read in the dataset (or its reverse complement).
  for (size_t i = nb_sequences; i > nb_correct; i--)
    for (size_t j = 0; j < nb_correct; j++) {
      wchar_t *rcs = 0;
      if ((i - 1) != j && hd (sequences[i - 1], sequences[j]) == 1)
        fprintf (stdout, "%ls->%ls\n", sequences[i - 1], sequences[j]);
      else if ((i - 1) != j && hd (sequences[i - 1], (rcs = rc (sequences[j]))) == 1)
        fprintf (stdout, "%ls->%ls\n", sequences[i - 1], rcs);
      else
        continue;
      wchar_t *tmp = sequences[i - 1];
      sequences[i - 1] = sequences[nb_sequences - nb_incorrect - 1];
      sequences[nb_sequences - nb_incorrect++ - 1] = tmp;
      break;
    }

  assert (nb_correct + nb_incorrect <= nb_sequences);

  for (size_t i = 0; i < nb_sequences; i++)
    free (sequences[i]);
  free (sequences);
}
