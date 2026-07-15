#include "rosalind.h"

struct tDNA {
  wchar_t *anticodon;
  wchar_t *aminoacid;
};

static wchar_t *const PROT_AA;
static wchar_t *const START_AA = L"M";
static wchar_t *const STOP_AA = L"\n";

// An open reading frame (ORF) is one which starts from the start codon and ends by stop codon, without any other stop codons in between.
//  Thus, a candidate protein string is derived by translating an open reading frame into amino acids until a stop codon is reached.

static struct tDNA tStart[] = {
  { L"ATG", START_AA },
};
static struct tDNA tStop[] = {
  { L"TAA", STOP_AA },
  { L"TAG", STOP_AA },
  { L"TGA", STOP_AA },
};
static struct tDNA tDNA[] = {
  { L"ATG", L"M" },    // Can also be considered as an amino-acid in an ORF.
  { L"TAA", STOP_AA }, // Stops are included in the protein chain
  { L"TAG", STOP_AA },
  { L"TGA", STOP_AA },
  { L"TTT", L"F" },
  { L"CTT", L"L" },
  { L"ATT", L"I" },
  { L"GTT", L"V" },
  { L"TTC", L"F" },
  { L"CTC", L"L" },
  { L"ATC", L"I" },
  { L"GTC", L"V" },
  { L"TTA", L"L" },
  { L"CTA", L"L" },
  { L"ATA", L"I" },
  { L"GTA", L"V" },
  { L"TTG", L"L" },
  { L"CTG", L"L" },
  { L"GTG", L"V" },
  { L"TCT", L"S" },
  { L"CCT", L"P" },
  { L"ACT", L"T" },
  { L"GCT", L"A" },
  { L"TCC", L"S" },
  { L"CCC", L"P" },
  { L"ACC", L"T" },
  { L"GCC", L"A" },
  { L"TCA", L"S" },
  { L"CCA", L"P" },
  { L"ACA", L"T" },
  { L"GCA", L"A" },
  { L"TCG", L"S" },
  { L"CCG", L"P" },
  { L"ACG", L"T" },
  { L"GCG", L"A" },
  { L"TAT", L"Y" },
  { L"CAT", L"H" },
  { L"AAT", L"N" },
  { L"GAT", L"D" },
  { L"TAC", L"Y" },
  { L"CAC", L"H" },
  { L"AAC", L"N" },
  { L"GAC", L"D" },
  { L"CAA", L"Q" },
  { L"AAA", L"K" },
  { L"GAA", L"E" },
  { L"CAG", L"Q" },
  { L"AAG", L"K" },
  { L"GAG", L"E" },
  { L"TGT", L"C" },
  { L"CGT", L"R" },
  { L"AGT", L"S" },
  { L"GGT", L"G" },
  { L"TGC", L"C" },
  { L"CGC", L"R" },
  { L"AGC", L"S" },
  { L"GGC", L"G" },
  { L"CGA", L"R" },
  { L"AGA", L"R" },
  { L"GGA", L"G" },
  { L"TGG", L"W" },
  { L"CGG", L"R" },
  { L"AGG", L"R" },
  { L"GGG", L"G" },
};

static size_t
beginswith (wchar_t *dna, wchar_t *aa_class) {
  struct tDNA *t;
  size_t l = 0;
  if (aa_class == PROT_AA) {
    t = tDNA;
    l = lengthof (tDNA);
  } else if (aa_class == START_AA) {
    t = tStart;
    l = lengthof (tStart);
  } else if (aa_class == STOP_AA) {
    t = tStop;
    l = lengthof (tStop);
  }
  for (struct tDNA *p = t; p < t + l; p++)
    if (!wcsncmp (dna, p->anticodon, wcslen (p->anticodon)))
      return wcslen (p->anticodon);
  return 0;
}
// AGCCATGTAGCTAACTCAGGTTACATGGGGATGACCCCGCGACTTGGATTAGAGTCTCTTTTGGAATAAGCCTGAATGATCCGAGTAGCATCTCAG
//     ATGTAG
//                         ATGGGGATGACCCCGCGACTTGGATTAGAGTCTCTTTTGGAATAA
//                               ATGACCCCGCGACTTGGATTAGAGTCTCTTTTGGAATAA
//
// CTGAGATGCTACTCGGATCATTCAGGCTTATTCCAAAAGAGACTCTAATCCAAGTCGCGGGGTCATCCCCATGTAACCTGAGTTAGCTACATGGCT
//      ATGCTACTCGGATCATTCAGGCTTATTCCAAAAGAGACTCTAATCCAAGTCGCGGGGTCATCCCCATGTAACCTGAGTTAG
//                                                                       ATGTAA
static void
dnatoprot (wchar_t *start, wchar_t *end) {
  size_t aa_length = 0;
  for (wchar_t *aa = start; aa < end; aa += aa_length)
    for (struct tDNA *p = tDNA; p < tDNA + lengthof (tDNA); p++)
      if (!wcsncmp (aa, p->anticodon, wcslen (p->anticodon))) {
        aa_length = wcslen (p->anticodon);
        fprintf (stdout, "%ls", p->aminoacid);
        break;
      }
}

static void
searchdna (wchar_t *dna) {
  size_t aa_length;
  for (wchar_t *start = dna; *start; start++)
    if (!beginswith (start, START_AA))
      continue;
    else
      for (wchar_t *aa = start; *aa; aa += aa_length)
        if ((aa_length = beginswith (aa, STOP_AA))) {
          dnatoprot (start, aa + aa_length);
          break;
        } else if (!(aa_length = beginswith (aa, PROT_AA)))
          break;
}

static wchar_t
getdnacomplement (wchar_t letter) {
  switch (letter) {
  case L'A':
    return L'T';
  case L'T':
    return L'A';
  case L'C':
    return L'G';
  case L'G':
    return L'C';
  default:
    exit (EXIT_FAILURE);
  }
}

static void
reversecomplementdna (wchar_t *dna) {
  if (*dna == L'\0')
    return;
  wchar_t *p1, *p2, tmp;
  for (p1 = dna, p2 = dna + wcslen (dna) - 1; p1 < p2; p1++, p2--) {
    tmp = *p1;
    *p1 = getdnacomplement (*p2);
    *p2 = getdnacomplement (tmp);
  }
}

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *dna = 0;
  if (!freadFASTA (0, &dna, stdin))
    return EXIT_FAILURE;

  searchdna (dna);
  reversecomplementdna (dna);
  searchdna (dna);

  free (dna);
}
