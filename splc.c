#include "rosalind.h"

struct tRNA {
  wchar_t *anticodon;
  wchar_t *aminoacid;
};

static struct tRNA tRNA[] = {
  { L"UUU", L"F" },
  { L"CUU", L"L" },
  { L"AUU", L"I" },
  { L"GUU", L"V" },
  { L"UUC", L"F" },
  { L"CUC", L"L" },
  { L"AUC", L"I" },
  { L"GUC", L"V" },
  { L"UUA", L"L" },
  { L"CUA", L"L" },
  { L"AUA", L"I" },
  { L"GUA", L"V" },
  { L"UUG", L"L" },
  { L"CUG", L"L" },
  { L"AUG", L"M" /* Start */ },
  { L"GUG", L"V" },
  { L"UCU", L"S" },
  { L"CCU", L"P" },
  { L"ACU", L"T" },
  { L"GCU", L"A" },
  { L"UCC", L"S" },
  { L"CCC", L"P" },
  { L"ACC", L"T" },
  { L"GCC", L"A" },
  { L"UCA", L"S" },
  { L"CCA", L"P" },
  { L"ACA", L"T" },
  { L"GCA", L"A" },
  { L"UCG", L"S" },
  { L"CCG", L"P" },
  { L"ACG", L"T" },
  { L"GCG", L"A" },
  { L"UAU", L"Y" },
  { L"CAU", L"H" },
  { L"AAU", L"N" },
  { L"GAU", L"D" },
  { L"UAC", L"Y" },
  { L"CAC", L"H" },
  { L"AAC", L"N" },
  { L"GAC", L"D" },
  { L"UAA", L"\n" /* Stop */ },
  { L"CAA", L"Q" },
  { L"AAA", L"K" },
  { L"GAA", L"E" },
  { L"UAG", L"\n" /* Stop */ },
  { L"CAG", L"Q" },
  { L"AAG", L"K" },
  { L"GAG", L"E" },
  { L"UGU", L"C" },
  { L"CGU", L"R" },
  { L"AGU", L"S" },
  { L"GGU", L"G" },
  { L"UGC", L"C" },
  { L"CGC", L"R" },
  { L"AGC", L"S" },
  { L"GGC", L"G" },
  { L"UGA", L"\n" /* Stop */ },
  { L"CGA", L"R" },
  { L"AGA", L"R" },
  { L"GGA", L"G" },
  { L"UGG", L"W" },
  { L"CGG", L"R" },
  { L"AGG", L"R" },
  { L"GGG", L"G" },
};

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *dna;
  freadFASTA (0, &dna, stdin);

  size_t nbintrons = 0;
  wchar_t **intron = 0;
  wchar_t *genetic = 0;
  while (freadFASTA (0, &genetic, stdin)) {
    intron = realloc (intron, ++nbintrons * sizeof (*intron));
    intron[nbintrons - 1] = genetic;
  }

  fprintf (stderr, "%ls\n-\n", dna);
  for (size_t i = 0; i < nbintrons; i++)
    fprintf (stderr, "%ls\n", intron[i]);
  fprintf (stderr, "-\n");

  wchar_t *mRNA = malloc ((wcslen (dna) + 1) * sizeof (*mRNA));
  wchar_t *next, *haystack;
  for (size_t i = 0; i < nbintrons; i++) {
    wcscpy (mRNA, L"");
    for (haystack = dna; (next = wcsstr (haystack, intron[i])); haystack = next + wcslen (intron[i])) {
      fprintf (stderr, "%ls + %ls[0:%zu[\n", mRNA, haystack, (size_t)(next - haystack));
      wcsncat (mRNA, haystack, (size_t)(next - haystack));
      fprintf (stderr, "%ls - %ls = %ls\n", dna, intron[i], mRNA);
    }
    fprintf (stderr, "%ls + %ls", mRNA, haystack);
    wcscat (mRNA, haystack);
    fprintf (stderr, " = %ls\n", mRNA);
    wcscpy (dna, mRNA);
    fprintf (stderr, "%ls\n-\n", dna);
  }
  fprintf (stderr, "%ls\n", dna);
  wcscpy (mRNA, dna);
  for (size_t i = 0; i < wcslen (mRNA); i++)
    if (mRNA[i] == L'T')
      mRNA[i] = L'U';
  fprintf (stderr, "%ls\n", mRNA);

  for (size_t i = 0; i < nbintrons; i++)
    free (intron[i]);
  free (intron);
  free (dna);

  ACMachine *ribosome = acm_create (ACM_CMP_DEFAULT, &(size_t){ sizeof (tRNA[0].anticodon[0]) }, 0);
  ACState *si = acm_initiate (ribosome);
  for (size_t i = 0; i < sizeof (tRNA) / sizeof (*tRNA); i++) {
    for (size_t j = 0; j < wcslen (tRNA[i].anticodon); j++)
      acm_insert_letter_of_keyword (&si, &tRNA[i].anticodon[j]);
    acm_insert_end_of_keyword (&si, tRNA[i].aminoacid, 0);
  }
  MatchHolder m;
  acm_matcher_init (&m);
  fprintf (stdout, "\n");
  const ACState *sm = acm_initiate (ribosome);
  for (wchar_t *wc = mRNA; *wc; wc++)
    if (acm_match (&sm, wc)) {
      acm_get_match (sm, 0, &m);
      fprintf (stdout, "%ls", (wchar_t *)m.value);
      fflush (stdout);
      sm = acm_initiate (ribosome);
    }
  acm_matcher_release (&m);
  acm_release (ribosome);

  free (mRNA);
}
