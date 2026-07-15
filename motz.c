#include "rosalind.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
// #define on_exit(f, a) ((void)(f), 0)

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
release_nms (int ret, void *arg) {
  (void)ret;
  map *nms = *(map **)arg;
  map_traverse (nms, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (nms);
}

static wchar_t
complement (wchar_t base) {
  return (base == L'A' ? L'U' : (base == L'C' ? L'G' : (base == L'G' ? L'C' : L'A')));
}

static uintbig_t
noncrossing_matchings (wchar_t *start, wchar_t *end /* excluded */) {
  static map *nms = 0;
  (void)get_key;
  (void)cmp_key;
  if (!nms) {
    assert (!on_exit (release_nms, &nms));
    nms = map_create (get_key, cmp_key, 0, 1);
  }
  if (end == start)
    return 1;
  assert (end > start);
  uintbig_t ret = 0;
  struct fragment *f = 0;
  if (nms && map_find_key (nms, &(struct fragment){ .start = start, .length = (size_t)(end - start) }, MAP_GET_ONE, &f, 0, 0))
    ret = f->npm;
  else {
    ret += noncrossing_matchings (start + 1, end);
    for (wchar_t *to = start + 1; *to && to < end; to++)
      if (*to == complement (*start))
        ret += noncrossing_matchings (start + 1, to) * noncrossing_matchings (to + 1, end);
    if (nms) {
      f = malloc (sizeof (*f));
      *f = (struct fragment){ .start = start, .length = (size_t)(end - start), .npm = ret };
      if (!map_insert_data (nms, f))
        free (f);
    }
  }
  return ret;
}

int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();

  wchar_t *genetic = 0;
  assert (freadFASTA (0, &genetic, stdin));

  uintbig_t nm = noncrossing_matchings (genetic, genetic + wcslen (genetic));
  fprintf (stderr, "%U\n", nm);
  fprintf (stdout, "%U", nm % 1000000);

  free (genetic);
}
