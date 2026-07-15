#include "map.h"
#include "rosalind.h"
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

struct dna {
  wint_t letter;
  size_t count;
};

static const void *
get_letter (void *data) {
  return &((const struct dna *)data)->letter;
}

static int
cmp_letter (const void *p1, const void *p2, const void *arg) {
  (void)arg;
  return *(const wint_t *)p1 > *(const wint_t *)p2 ? 1 : (*(const wint_t *)p1 < *(const wint_t *)p2 ? -1 : 0);
}

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *wcs = 0;
  wchar_t max_sample[100] = L"";
  wchar_t sample[100] = L"";
  double max_gc = 0;
  size_t gc = 0;
  size_t dna_length = 0;
  map *letters = map_create (get_letter, cmp_letter, 0, 1);
  const wchar_t *ret;
  while (1)
    if (!(ret = fgetwcs (&wcs, 0, stdin)) || *wcs == L'>') {
      struct dna *l;
      gc += map_find_key (letters, &(wint_t){ L'C' }, MAP_GET_ONE, &l, 0, 0) ? l->count : 0;
      gc += map_find_key (letters, &(wint_t){ L'G' }, MAP_GET_ONE, &l, 0, 0) ? l->count : 0;
      dna_length = gc;
      dna_length += map_find_key (letters, &(wint_t){ L'A' }, MAP_GET_ONE, &l, 0, 0) ? l->count : 0;
      dna_length += map_find_key (letters, &(wint_t){ L'T' }, MAP_GET_ONE, &l, 0, 0) ? l->count : 0;
      dna_length += map_find_key (letters, &(wint_t){ L'U' }, MAP_GET_ONE, &l, 0, 0) ? l->count : 0;
      if ((double)gc / (double)dna_length > max_gc) {
        wcscpy (max_sample, sample);
        max_gc = (double)gc / (double)dna_length;
      }
      gc = 0;
      map_traverse (letters, MAP_REMOVE_ALL, free, 0, 0);
      if (ret && *wcs == L'>')
        wcsncpy (sample, wcs + 1, sizeof (sample) / sizeof (*sample) - 1);
      else if (!ret)
        break;
    } else {
      struct dna *l;
      for (size_t i = wcslen (wcs); i; i--)
        if (map_find_key (letters, wcs + i - 1, MAP_GET_ONE, &l, 0, 0))
          l->count++;
        else {
          l = malloc (sizeof (*l));
          *l = (struct dna){ .letter = (wint_t)wcs[i - 1], .count = 1 };
          if (!map_insert_data (letters, l))
            free (l);
        }
    }
  map_destroy (letters);
  fprintf (stdout, "%ls%f\n", max_sample, 100. * max_gc);
}
