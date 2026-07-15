#include "rosalind.h"
#define min(a, b) ((a) < (b) ? (a) : (b))
/*
A = a * m + b.
C = c * m + d.
A * C = a * c * m * m + (a * d + b * c) * m + b * d.
A * C = p * m + (b * d).
(A * C) % m = (b * d) % m.

A + C = (a + c) * m + (b + d).
(A + C) % m = (b + d) % m.

A * C = q * D.
A * C / D = q.
*/

static const uintbig_t modulo = 1000000;
static uintbig_t
nb_comb (unsigned long n, unsigned long p) {
  assert (p <= n);
  if (2 * p > n)
    p = n - p;
  uintbig_t C = 1;
  for (unsigned long i = 0; i < p; i++) {
    uintbig_t product = C * (n - i);
    assert (C == 0 || (product / C) == (n - i));
    C = product / (i + 1);
  }
  return C;
}

struct pascal_triangle_elem {
  struct pte_key {
    unsigned long n, p;
  } key;
  uintbig_t nCp;
};

static const void *
get_pte_key (void *data) {
  return &((struct pascal_triangle_elem *)data)->key;
}

static int
cmp_pte_key (const void *key_a, const void *key_b, const void *arg) {
  (void)arg;
  const struct pte_key *a = key_a;
  const struct pte_key *b = key_b;
  return a->n < b->n ? -1 : (a->n > b->n ? 1 : (a->p < b->p ? -1 : (a->p > b->p ? 1 : 0)));
}

static map *pascal_triangle = 0;
static void
pte_clear (void) {
  map_traverse (pascal_triangle, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (pascal_triangle);
}
static uintbig_t
nb_comb_pascal_triangle_rec (unsigned long n, unsigned long p) {
  // nCp = (n-1)C(p-1) + (n-1)Cp.
  if (!pascal_triangle) {
    pascal_triangle = map_create (get_pte_key, cmp_pte_key, 0, 1);
    atexit (pte_clear);
  }
  assert (p <= n);
  if (2 * p > n)
    p = n - p;
  if (p == 0)
    return 1;
  assert (0 < p && p < n);
  fprintf (stderr, "%lu %lu\n", n, p);
  void *data;
  if (map_find_key (pascal_triangle, &(struct pte_key){ .n = n, .p = p }, MAP_GET_ONE, &data, 0, 0))
    return ((struct pascal_triangle_elem *)data)->nCp;
  else {
    struct pascal_triangle_elem *new = malloc (sizeof (*new));
    *new = (struct pascal_triangle_elem){ .key = { .n = n, .p = p }, .nCp = nb_comb_pascal_triangle_rec (n - 1, p - 1) + nb_comb_pascal_triangle_rec (n - 1, p) };
    assert (map_insert_data (pascal_triangle, new));
    return new->nCp;
  }
}

static uintbig_t
nb_comb_pascal_triangle (unsigned long n, unsigned long p) {
  // nCp = (n-1)C(p-1) + (n-1)Cp (see https://en.wikipedia.org/wiki/Combination)
  if (!pascal_triangle) {
    pascal_triangle = map_create (get_pte_key, cmp_pte_key, 0, 1);
    struct pascal_triangle_elem *new = malloc (sizeof (*new));
    *new = (struct pascal_triangle_elem){ .key = { .n = 0, .p = 0 }, .nCp = 1 };
    assert (map_insert_data (pascal_triangle, new));
    atexit (pte_clear);
  }
  assert (p <= n);
  if (2 * p > n)
    p = n - p;
  void *data;
  if (map_find_key (pascal_triangle, &(struct pte_key){ .n = n, .p = p }, MAP_GET_ONE, &data, 0, 0))
    return ((struct pascal_triangle_elem *)data)->nCp;
  if (p == 0)
    return 1;
  unsigned long startn = 1;
  for (; map_find_key (pascal_triangle, &(struct pte_key){ .n = startn, .p = 0 }, MAP_EXISTS_ONE, 0, 0, 0); startn++)
    ;
  if (startn > 1)
    startn--; // Start from where the last call to nb_comb_pascal_triangle stopped.
  for (unsigned long in = startn; in <= n; in++) {
    fprintf (stderr, "%lu %lu\n", in, 0);
    if (!map_find_key (pascal_triangle, &(struct pte_key){ .n = in, .p = 0 }, MAP_EXISTS_ONE, 0, 0, 0)) {
      struct pascal_triangle_elem *new = malloc (sizeof (*new));
      *new = (struct pascal_triangle_elem){ .key = { .n = in, .p = 0 }, .nCp = 1 };
      assert (map_insert_data (pascal_triangle, new));
    }
    for (unsigned long ip = 1; ip <= in / 2; ip++) {
      fprintf (stderr, "%lu %lu\n", in, ip);
      assert (0 < ip && ip < in);
      uintbig_t nCp = 0;
      assert (map_find_key (pascal_triangle, &(struct pte_key){ .n = in - 1, .p = ip - 1 }, MAP_GET_ONE, &data, 0, 0));
      nCp += ((struct pascal_triangle_elem *)data)->nCp;
      assert (map_find_key (pascal_triangle, &(struct pte_key){ .n = in - 1, .p = min (ip, in - ip - 1) }, MAP_GET_ONE, &data, 0, 0));
      nCp += ((struct pascal_triangle_elem *)data)->nCp;
      if (!map_find_key (pascal_triangle, &(struct pte_key){ .n = in, .p = ip }, MAP_EXISTS_ONE, 0, 0, 0)) {
        struct pascal_triangle_elem *new = malloc (sizeof (*new));
        *new = (struct pascal_triangle_elem){ .key = { .n = in, .p = ip }, .nCp = nCp % modulo }; // A modulo could be applied here.
        assert (map_insert_data (pascal_triangle, new));
      }
      if (in == n && ip == p)
        return nCp;
    }
  }
  return 1;
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();

  wchar_t *n_and_m = 0;
  assert (fgetwcs (&n_and_m, 0, stdin) && n_and_m && *n_and_m);

  wchar_t *token;
  wchar_t *nexttok = n_and_m;
  assert ((token = wcstok (0, L" \t\n", &nexttok)));
  wchar_t *ptr;
  unsigned long n = wcstoul (token, &ptr, 10);
  assert (ptr != token);

  assert ((token = wcstok (0, L" \t\n", &nexttok)));
  unsigned long m = wcstoul (token, &ptr, 10);
  assert (ptr != token);

  free (n_and_m);
  fprintf (stderr, "%lu %lu\n", n, m);

  (void)nb_comb_pascal_triangle;
  (void)nb_comb_pascal_triangle_rec; // NOK. Too many levels. Stack overflow.
  (void)nb_comb;
  uintbig_t (*Cnk) (unsigned long n, unsigned long p) = nb_comb_pascal_triangle;
  uintbig_t sum = 0;
  for (unsigned long k = m; k <= n; k++)
    sum += Cnk (n, k);
  fprintf (stdout, "%U\n", sum % modulo);
  sum = 0;
  for (unsigned long k = m + 1; k <= n + 1; k++)
    sum += Cnk (n + 1, k);
  fprintf (stderr, "%U\n", sum % modulo);
}
