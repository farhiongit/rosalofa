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
  assert (freadFASTA (0, &dna1, stdin));
  wchar_t *dna2 = 0;
  assert (freadFASTA (0, &dna2, stdin));

  size_t *lcsq_grid = calloc ((wcslen (dna1) + 1) * (wcslen (dna2) + 1), sizeof (*lcsq_grid));
#ifdef DISCOVER_STRATEGY
  map *csqs = map_create (0, 0, 0, 0); // queue
  csq_t *start = malloc (sizeof (*start));
  *start = (csq_t){ .length = 0, .end1 = dna1, .end2 = dna2 };
  assert (map_insert_data (csqs, start));

  for (void *data; map_traverse (csqs, MAP_REMOVE_ONE, &data, 0, 0); free (data)) {
    csq_t *csq = data;
    // fprintf (stderr, "(%zu, %zu) = %zu\n", (size_t)(csq->end1 - dna1), (size_t)(csq->end2 - dna2), csq->length);
    if (*csq->end1 && csq->length > lcsq_grid[(wcslen (dna1) + 1) * (size_t)(csq->end2 - dna2) + (size_t)(csq->end1 + 1 - dna1)]) {
      csq_t *new = malloc (sizeof (*new));
      *new = (csq_t){ .length = csq->length, .end1 = csq->end1 + 1, .end2 = csq->end2 };
      assert (map_insert_data (csqs, new));
      lcsq_grid[(wcslen (dna1) + 1) * (size_t)(csq->end2 - dna2) + (size_t)(csq->end1 + 1 - dna1)] = csq->length;
    }
    if (*csq->end2 && csq->length > lcsq_grid[(wcslen (dna1) + 1) * (size_t)(csq->end2 + 1 - dna2) + (size_t)(csq->end1 - dna1)]) {
      csq_t *new = malloc (sizeof (*new));
      *new = (csq_t){ .length = csq->length, .end1 = csq->end1, .end2 = csq->end2 + 1 };
      assert (map_insert_data (csqs, new));
      lcsq_grid[(wcslen (dna1) + 1) * (size_t)(csq->end2 + 1 - dna2) + (size_t)(csq->end1 - dna1)] = csq->length;
    }
    if (*csq->end1 && *csq->end2 && *csq->end1 == *csq->end2) {
      if (csq->length + 1 > lcsq_grid[(wcslen (dna1) + 1) * (size_t)(csq->end2 + 1 - dna2) + (size_t)(csq->end1 + 1 - dna1)]) {
        csq_t *new = malloc (sizeof (*new));
        *new = (csq_t){ .length = csq->length + 1, .end1 = csq->end1 + 1, .end2 = csq->end2 + 1 };
        assert (map_insert_data (csqs, new));
        lcsq_grid[(wcslen (dna1) + 1) * (size_t)(csq->end2 + 1 - dna2) + (size_t)(csq->end1 + 1 - dna1)] = csq->length + 1;
      }
    }
  } // for (void *data; map_traverse ...
  map_destroy (csqs);
#else
  for (size_t i = 1; i <= wcslen (dna1); i++)
    for (size_t j = 1; j <= wcslen (dna2); j++)
      lcsq_grid[(wcslen (dna1) + 1) * j + i] = max (max (lcsq_grid[(wcslen (dna1) + 1) * j + (i - 1)], lcsq_grid[(wcslen (dna1) + 1) * (j - 1) + i]),
                                                    lcsq_grid[(wcslen (dna1) + 1) * (j - 1) + (i - 1)] + (dna1[i - 1] == dna2[j - 1]));
#endif

  // Show grid.
  fprintf (stderr, "  ");
  for (size_t j = 0; j < wcslen (dna2); j++)
    fprintf (stderr, "   %lc", (wint_t)dna2[j]);
  fprintf (stderr, "\n");
  for (size_t i = 0; i < wcslen (dna1); i++) {
    fprintf (stderr, "%lc ", (wint_t)dna1[i]);
    for (size_t j = 0; j < wcslen (dna2) + 1; j++)
      fprintf (stderr, " %3zu", lcsq_grid[(wcslen (dna1) + 1) * j + i]);
    fprintf (stderr, "\n");
  }
  fprintf (stderr, "  ");
  for (size_t j = 0; j < wcslen (dna2) + 1; j++)
    fprintf (stderr, " %3zu", lcsq_grid[(wcslen (dna1) + 1) * j + wcslen (dna1)]);
  fprintf (stderr, "\n");

  // Get length
  size_t lcsq_length = lcsq_grid[(wcslen (dna1) + 1) * wcslen (dna2) + wcslen (dna1)];
  fprintf (stderr, "%zu\n", lcsq_length);

  // Trace back one solution.
  wchar_t *lscq = calloc (lcsq_length + 1, sizeof (*lscq));
  size_t i = wcslen (dna1);
  size_t j = wcslen (dna2);
  while (lcsq_length) {
    if (i && j && dna1[i - 1] == dna2[j - 1] && lcsq_grid[(wcslen (dna1) + 1) * j + i] == lcsq_grid[(wcslen (dna1) + 1) * (j - 1) + (i - 1)] + 1) {
      lscq[--lcsq_length] = dna1[i - 1];
      i--;
      j--;
    } else if (i && lcsq_grid[(wcslen (dna1) + 1) * j + i] == lcsq_grid[(wcslen (dna1) + 1) * j + (i - 1)])
      i--;
    else if (j && lcsq_grid[(wcslen (dna1) + 1) * j + i] == lcsq_grid[(wcslen (dna1) + 1) * (j - 1) + i])
      j--;
    else
      break;
  }
  assert (!lcsq_length);
  fprintf (stdout, "%ls\n", lscq);
  free (lscq);
  free (lcsq_grid);
  free (dna2);
  free (dna1);
}
