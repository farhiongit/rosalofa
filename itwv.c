#include "rosalind.h"

struct fifo {
  size_t spos, tpos, upos;
};

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *dna = 0;
  assert (fgetwcs (&dna, 0, stdin));

  wchar_t **motif = 0;
  size_t nb_motifs = 0;
  wchar_t *line = 0;
  while (fgetwcs (&line, 0, stdin)) {
    size_t l = wcslen (line);
    assert (l);
    if (line[l - 1] == L'\n')
      line[l - 1] = 0;
    assert (wcslen (line));
    motif = realloc (motif, (++nb_motifs) * sizeof (*motif));
    motif[nb_motifs - 1] = wcsdup (line);
  }
  free (line);

  wchar_t *s = dna;
  size_t ls = wcslen (s);
  int *M = calloc (nb_motifs * nb_motifs, sizeof (*M));
  for (size_t k = 0; k < nb_motifs; k++) {
    for (size_t j = k; j < nb_motifs; j++) {
      wchar_t *t = motif[k];
      wchar_t *u = motif[j];
      size_t lt = wcslen (t);
      size_t lu = wcslen (u);

      if (ls < lt + lu)
        continue;

      map *fifo = map_create (0, 0, 0, 0);

      for (size_t i = 0; i < ls - lt - lu; i++) {
        struct fifo *e = calloc (1, sizeof (*e));
        e->spos = i;
        assert (map_insert_data (fifo, e));
      }

      for (struct fifo *e; map_traverse (fifo, MAP_REMOVE_ONE, &e, 0, 0); free (e)) {
        if (e->spos < ls && e->tpos < lt && s[e->spos] == t[e->tpos]) {
          struct fifo *f = calloc (1, sizeof (*f));
          *f = (struct fifo){ .spos = e->spos + 1, .tpos = e->tpos + 1, .upos = e->upos };
          assert (map_insert_data (fifo, f));
        }
        if (e->spos < ls && e->upos < lu && s[e->spos] == u[e->upos]) {
          struct fifo *f = calloc (1, sizeof (*f));
          *f = (struct fifo){ .spos = e->spos + 1, .tpos = e->tpos, .upos = e->upos + 1 };
          assert (map_insert_data (fifo, f));
        }
        if (e->tpos == lt && e->upos == lu) {
          M[j + nb_motifs * k] = 1;
          map_traverse (fifo, MAP_REMOVE_ALL, free, 0, 0);
        }
      } // for (struct fifo *e; map_traverse...

      map_traverse (fifo, MAP_REMOVE_ALL, free, 0, 0);
      map_destroy (fifo);
    } // for (size_t j = k; j < nb_motifs; j++)
  } // for (size_t k = 0; k < nb_motifs; k++)

  for (size_t i = 0; i < nb_motifs; i++)
    free (motif[i]);
  free (motif);

  for (size_t k = 0; k < nb_motifs; k++) {
    for (size_t j = 0; j < nb_motifs; j++)
      fprintf (stdout, "%s%i", j ? " " : "", j >= k ? M[j + nb_motifs * k] : M[k + nb_motifs * j]);
    fprintf (stdout, "\n");
  }
  free (M);
  free (dna);
}
