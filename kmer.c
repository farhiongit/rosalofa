#include "rosalind.h"

static void
display_kmer (MatchHolder m) {
  static size_t nb_kmers;
  for (size_t i = 0; i < m.length; i++)
    fprintf (stderr, "%lc", *(const wint_t *)m.letters[i]);
  fprintf (stdout, "%s%zu", nb_kmers++ ? " " : "", *(size_t *)m.value);
  fflush (stdout);
  fprintf (stderr, "\n");
}

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *sequence = 0;
  freadFASTA (0, &sequence, stdin);
  fprintf (stderr, "%ls\n", sequence);
  wchar_t nucleotides[] = { L'A', L'C', L'G', L'T' };
  size_t nb_nucleotides = lengthof (nucleotides);

  ACMachine *kmers = acm_create (ACM_CMP_DEFAULT, &(size_t){ sizeof (wchar_t) }, 0);

  size_t k = 4;
  size_t *const kmer = calloc (k, sizeof (*kmer)); // substring
  // order all possible k-mers taken from an underlying alphabet lexicographically.
  ACState *insert = acm_initiate (kmers);
  for (int go = 1; go;) {
    for (size_t i = 0; i < k; i++) {
      fprintf (stderr, "%lc", (wint_t)nucleotides[kmer[i]]);
      acm_insert_letter_of_keyword (&insert, &nucleotides[kmer[i]]);
    }
    fprintf (stderr, "\n");
    acm_insert_end_of_keyword (&insert, calloc (1, sizeof (size_t)), free);
    go = 0;
    for (size_t i = 0; i < k; i++)
      if (kmer[k - i - 1] < nb_nucleotides - 1) {
        kmer[k - i - 1]++;
        go = 1;
        break;
      } else
        kmer[k - i - 1] = 0;
  }
  free (kmer);

  MatchHolder m;
  acm_matcher_init (&m);
  const ACState *matcher = acm_initiate (kmers);
  for (wchar_t *c = sequence; *c; c++)
    for (size_t nbmatch = acm_match (&matcher, c); nbmatch; nbmatch--) {
      acm_get_match (matcher, nbmatch - 1, &m);
      (*(size_t *)m.value)++;
    }
  acm_matcher_release (&m);

  acm_foreach_keyword (kmers, display_kmer);

  acm_release (kmers);
  free (sequence);
}
