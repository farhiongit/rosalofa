#include "rosalind.h"

static int
kmerscmp (const void *a, const void *b, const void *) {
  const wchar_t *sa = a;
  const wchar_t *sb = b;
  return wcscmp (sa, sb);
}

static int
print_adj_list (void *data, void *op_arg, int *remove, const void *context) {
  wchar_t *sa = data;
  fprintf (op_arg, "(");
  size_t l = wcslen (sa);
  for (size_t i = 1; i < l; i++)
    fprintf (op_arg, "%lc", (wint_t)sa[i - 1]);
  fprintf (op_arg, ", ");
  for (size_t i = 1; i < l; i++)
    fprintf (op_arg, "%lc", (wint_t)sa[i]);
  fprintf (op_arg, ")\n");
  void (*release) (void *) = context;
  release (data);
  *remove = 1;
  return 1;
}

#define COMPL(c) ((c) == L'A' ? L'T' : ((c) == L'T' ? L'A' : ((c) == L'C' ? L'G' : ((c) == L'G' ? L'C' : (c)))))
static void
revcomp (wchar_t *kmer) {
  wchar_t *p, *q;
  for (p = kmer, q = kmer + wcslen (kmer) - 1; *p && p < q; p++, q--) {
    wchar_t tmp = COMPL (*p);
    *p = COMPL (*q);
    *q = tmp;
  }
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  map *dbru = map_create (0, kmerscmp, 0, 1);
  map_set_context (dbru, free);

  wchar_t *line = 0;
  while (fgetwcs (&line, 0, stdin)) {
    if (line[wcslen (line) - 1] == L'\n')
      line[wcslen (line) - 1] = 0;
    fprintf (stderr, "%ls\n", line);
    wchar_t *kmer;
    kmer = wcsdup (line);
    if (!map_insert_data (dbru, kmer))
      free (kmer);
    revcomp (line);
    kmer = wcsdup (line);
    if (!map_insert_data (dbru, kmer))
      free (kmer);
  }
  free (line);

  map_traverse (dbru, print_adj_list, stdout, 0, 0);
  map_destroy (dbru);
}
