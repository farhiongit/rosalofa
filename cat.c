#include "rosalind.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
// #define on_exit(f, a) ((void)(f), 0)

static void
release_catnum (int ret, void *arg) {
  (void)ret;
  free (*(uintbig_t **)arg);
}

static uintbig_t
catalan_number (size_t n) {
  static size_t catnum_length = 0;
  static uintbig_t *catnum = 0;
  if (!catnum)
    assert (!on_exit (release_catnum, &catnum));
  if (n >= catnum_length) {
    catnum = realloc (catnum, (n + 1) * sizeof (*catnum));
    for (size_t i = catnum_length; i <= n && i < 2; i++)
      catnum[i] = 1;
    for (size_t i = max (2, catnum_length); i <= n; i++) {
      catnum[i] = 0;
      for (size_t k = 1; k <= i; k++)
        catnum[i] += catnum[k - 1] * catnum[i - k];
    }
    catnum_length = n + 1;
  }
  return catnum[n];
}

struct fragment {
  wchar_t *start;
  size_t length;
  uintbig_t npm;
};

static const void *
get_key (void *data) {
  return data;
}

static int
cmp_key (const void *key_a, const void *key_b, const void *arg) {
  (void)arg;
  const struct fragment *fa = key_a;
  const struct fragment *fb = key_b;
  assert (fa->length && fb->length);
  int ret = wcsncmp (fa->start, fb->start, min (fa->length, fb->length));
  return ret ? ret : (fa->length < fb->length ? -1 : (fa->length > fb->length ? 1 : 0));
}

static void
release_npms (int ret, void *arg) {
  (void)ret;
  map *npms = *(map **)arg;
  map_traverse (npms, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (npms);
}

static wchar_t
complement (wchar_t base) {
  return (base == L'A' ? L'U' : (base == L'C' ? L'G' : (base == L'G' ? L'C' : L'A')));
}

static uintbig_t
noncrossing_perfect_matchings (wchar_t *start, wchar_t *end /* excluded */) {
  static map *npms = 0;
  (void)get_key;
  (void)cmp_key;
  if (!npms) {
    assert (!on_exit (release_npms, &npms));
    npms = map_create (get_key, cmp_key, 0, 1);
  }
  if (end == start)
    return 1;
  assert (end > start && (end - start) % 2 == 0);
  uintbig_t ret = 0;
  struct fragment *f = 0;
  if (npms && map_find_key (npms, &(struct fragment){ .start = start, .length = (size_t)(end - start) }, MAP_GET_ONE, &f, 0, 0))
    ret = f->npm;
  else {
    size_t nA = 0;
    size_t nC = 0;
    size_t nG = 0;
    size_t nU = 0;
    for (wchar_t *to = start; *to && to < end; to++) {
      nA += (*to == L'A');
      nC += (*to == L'C');
      nG += (*to == L'G');
      nU += (*to == L'U');
      if (*to == complement (*start) && nA == nU && nC == nG)
        ret += noncrossing_perfect_matchings (start + 1, to) * noncrossing_perfect_matchings (to + 1, end);
    }
    if (npms) {
      f = malloc (sizeof (*f));
      *f = (struct fragment){ .start = start, .length = (size_t)(end - start), .npm = ret };
      if (!map_insert_data (npms, f))
        free (f);
    }
  }
  return ret;
}

static int
check_genetic (const wchar_t *const genetic) {
  long dAU = 0;
  long dCG = 0;
  size_t invalid = 0;
  for (const wchar_t *c = genetic; *c; c++)
    if (*c == L'A')
      dAU++;
    else if (*c == L'U')
      dAU--;
    else if (*c == L'C')
      dCG++;
    else if (*c == L'G')
      dCG--;
    else
      invalid++;
  return (dAU == 0 && dCG == 0 && invalid == 0);
}

static void
test_catnum (void) {
  const size_t n = 200;
  for (size_t i = 0; i <= n; i++)
    fprintf (stderr, "%U\n", catalan_number (i));
  for (size_t i = 0; i <= n; i++)
    fprintf (stderr, "%U\n", catalan_number (n - i));
}

static void
test_npm (void) {
  wchar_t *rna = L"AUAUAU";
  assert (check_genetic (rna));
  fprintf (stderr, "%U\n", noncrossing_perfect_matchings (rna, rna + wcslen (rna)));
}

int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();

  test_catnum ();
  fprintf (stderr, "\n");
  test_npm ();

  wchar_t *genetic = 0;
  assert (freadFASTA (0, &genetic, stdin) && check_genetic (genetic));

  uintbig_t npm = noncrossing_perfect_matchings (genetic, genetic + wcslen (genetic));
  fprintf (stderr, "%U\n", npm);
  fprintf (stdout, "%U", npm % 1000000);

  free (genetic);
}
