#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *line = 0;
  fgetwcs (&line, 0, stdin);
  size_t n = wcstoul (line, 0, 10);
  fprintf (stderr, "%zu\n", n);
  free (line);

  size_t nbperm = 1;
  for (size_t i = 1; i <= n; nbperm *= i++)
    ;
  fprintf (stdout, "%zu\n", nbperm);
  size_t *a = malloc (n * sizeof (*a));

  // https://en.wikipedia.org/wiki/Permutation#Algorithms_to_generate_permutations
  // Narayana Pandita
  // 1. Starting order
  for (size_t i = 1; i <= n; i++)
    a[i - 1] = i;
  for (size_t i = 0; i < n; i++)
    fprintf (stdout, "%s%zu", i == 0 ? "" : " ", a[i]);
  fprintf (stdout, "\n");
  while (1) {
    // Find the largest index k such that a[k] < a[k + 1]. If no such index exists, the permutation is the last permutation.
    size_t k;
    for (k = n - 1; k; k--)
      if (a[k - 1] < a[k])
        break;
    if (!k)
      break;
    else
      k--;
    // Find the largest index l greater than k such that a[k] < a[l].
    size_t l;
    for (l = n - 1; l > k; l--)
      if (a[l] > a[k])
        break;
    // Swap the value of a[k] with that of a[l].
    size_t tmp = a[k];
    a[k] = a[l];
    a[l] = tmp;
    // Reverse the sequence from a[k + 1] up to and including the final element a[n].
    size_t i, j;
    for (i = k + 1, j = n - 1; i < j; i++, j--) {
      tmp = a[i];
      a[i] = a[j];
      a[j] = tmp;
    }
    for (i = 0; i < n; i++)
      fprintf (stdout, "%s%zu", i == 0 ? "" : " ", a[i]);
    fprintf (stdout, "\n");
  }

  free (a);
}
