#include "rosalind.h"

static void
print_signed_perm (size_t n, size_t *a) {
  for (size_t j = 1UL << n; j; j--) {
    for (size_t i = 0; i < n; i++)
      fprintf (stdout, "%s%i", i == 0 ? "" : " ", (((1UL << i) & (j - 1)) ? 1 : -1) * (int)a[i]);
    fprintf (stdout, "\n");
  }
}

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
  fprintf (stdout, "%zu\n", nbperm * (1UL << n));
  size_t *a = malloc (n * sizeof (*a));

  // https://en.wikipedia.org/wiki/Permutation#Algorithms_to_generate_permutations
  // Narayana Pandita
  // 1. Starting order
  for (size_t i = 1; i <= n; i++)
    a[i - 1] = i;
  print_signed_perm (n, a);
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
    print_signed_perm (n, a);
  }

  free (a);
}
