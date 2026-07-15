#include "map.h"

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
  struct dna *l;

  map *letters = map_create (get_letter, cmp_letter, 0, 1);

  for (wint_t wc = 0; (wc = fgetwc (stdin)) != WEOF;) {
    if (map_find_key (letters, &wc, MAP_GET_ONE, &l, 0, 0))
      l->count++;
    else {
      l = malloc (sizeof (*l));
      *l = (struct dna){ .letter = wc, .count = 1 };
      if (!map_insert_data (letters, l))
        free (l);
    }
  }

  wint_t letter[] = { L'A', L'C', L'G', L'T' };
  for (size_t i = 0; i < sizeof (letter) / sizeof (*letter); i++)
    fprintf (stdout, "%2$s%1$zu", map_find_key (letters, letter + i, MAP_GET_ONE, &l, 0, 0) ? l->count : 0, i ? " " : "");
  fprintf (stdout, "\n");

  map_traverse (letters, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (letters);
}
