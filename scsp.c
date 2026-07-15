#include "rosalind.h"
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct _s_csq {
  wchar_t *end1;
  wchar_t *end2;
  size_t length;
} csq_t;

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *dna1 = 0;
  assert (fgetwcs (&dna1, 0, stdin));
  if (wcslen (dna1) && dna1[wcslen (dna1) - 1] == L'\n')
    dna1[wcslen (dna1) - 1] = 0;
  wchar_t *dna2 = 0;
  assert (fgetwcs (&dna2, 0, stdin));
  if (wcslen (dna2) && dna2[wcslen (dna2) - 1] == L'\n')
    dna2[wcslen (dna2) - 1] = 0;

  size_t *scsp_grid = calloc ((wcslen (dna1) + 1) * (wcslen (dna2) + 1), sizeof (*scsp_grid));
  for (size_t i = 1; i <= wcslen (dna1); i++)
    for (size_t j = 1; j <= wcslen (dna2); j++)
      scsp_grid[(wcslen (dna1) + 1) * j + i] = max (max (scsp_grid[(wcslen (dna1) + 1) * (j - 1) + (i)], scsp_grid[(wcslen (dna1) + 1) * (j) + (i - 1)]),
                                                    scsp_grid[(wcslen (dna1) + 1) * (j - 1) + (i - 1)] + (dna1[i - 1] == dna2[j - 1]));

  // Show grid.
  fprintf (stderr, "  ");
  for (size_t j = 0; j < wcslen (dna2); j++)
    fprintf (stderr, "   %lc", (wint_t)dna2[j]);
  fprintf (stderr, "\n");
  for (size_t i = 0; i < wcslen (dna1); i++) {
    fprintf (stderr, "%lc ", (wint_t)dna1[i]);
    for (size_t j = 0; j < wcslen (dna2) + 1; j++)
      fprintf (stderr, " %3zu", scsp_grid[(wcslen (dna1) + 1) * j + i]);
    fprintf (stderr, "\n");
  }
  fprintf (stderr, "  ");
  for (size_t j = 0; j < wcslen (dna2) + 1; j++)
    fprintf (stderr, " %3zu", scsp_grid[(wcslen (dna1) + 1) * j + wcslen (dna1)]);
  fprintf (stderr, "\n");

  // Get length
  size_t lcsq_length = wcslen (dna2) + wcslen (dna1) - scsp_grid[(wcslen (dna1) + 1) * wcslen (dna2) + wcslen (dna1)];
  fprintf (stderr, "%zu\n", lcsq_length);

  // Trace back one solution.
  wchar_t *lscq = calloc (lcsq_length + 1, sizeof (*lscq));
  size_t i = wcslen (dna1);
  size_t j = wcslen (dna2);
  while (lcsq_length) {
    if (i && j && dna1[i - 1] == dna2[j - 1] && scsp_grid[(wcslen (dna1) + 1) * j + i] == scsp_grid[(wcslen (dna1) + 1) * (j - 1) + (i - 1)] + 1) {
      lscq[--lcsq_length] = dna1[i - 1];
      i--;
      j--;
    } else if (i && scsp_grid[(wcslen (dna1) + 1) * j + i] == scsp_grid[(wcslen (dna1) + 1) * j + (i - 1)]) {
      lscq[--lcsq_length] = dna1[i - 1];
      i--;
    } else if (j && scsp_grid[(wcslen (dna1) + 1) * j + i] == scsp_grid[(wcslen (dna1) + 1) * (j - 1) + i]) {
      lscq[--lcsq_length] = dna2[j - 1];
      j--;
    } else
      break;
  }
  assert (!lcsq_length);
  fprintf (stdout, "%ls\n", lscq);
  free (lscq);
  free (scsp_grid);
  free (dna2);
  free (dna1);
}
