#include "rosalind.h"

struct tRNA {
  wchar_t *anticodon;
  wchar_t *aminoacid;
};

struct tRNA tRNA[] = {
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
  { L"AUG", L"M" },
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

static size_t
countCodons (wchar_t aminoacid) {
  size_t count = 0;
  for (size_t i = 0; i < lengthof (tRNA); i++)
    count += (*tRNA[i].aminoacid == aminoacid);
  return count;
}

int
main (void) {
  setlocale (LC_ALL, "");

  size_t modulo = 1000000;

  wchar_t *protein = 0;
  fgetwcs (&protein, 0, stdin);

  size_t nb = 1;
  for (size_t i = 0; i < wcslen (protein); i++)
    nb = (nb * countCodons (protein[i])) % modulo;
  if (protein[wcslen (protein) - 1] != L'\n')
    nb = (nb * countCodons (L'\n')) % modulo;
  fprintf (stdout, "%zu\n", nb);

  free (protein);
}
