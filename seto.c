#include "rosalind.h"

static int
fprint_elem (void *data, void *op_arg, int *, const void *context) {
  if (op_arg && context) {
    fprintf (op_arg, context, *(size_t *)data);
    fprintf (op_arg, ", ");
  }
  return 1;
}

static int
MAP_ALSO_IN (const void *data, void *sel_arg, const void *) {
  map *B = sel_arg;
  return map_find_key (B, data, MAP_EXISTS_ONE, 0, 0, 0) != 0;
}

static int
MAP_NOT_IN (const void *data, void *sel_arg, const void *) {
  map *B = sel_arg;
  return map_find_key (B, data, MAP_EXISTS_ONE, 0, 0, 0) == 0;
}

static size_t
fill_set (map *S, FILE *f) {
  wchar_t *line = 0;
  assert (fgetwcs (&line, 0, f));
  wchar_t *token;
  for (wchar_t *state = line; (token = wcstok (0, L"{}, \t\n", &state));) {
    size_t *e = malloc (sizeof (*e));
    wchar_t *end;
    *e = wcstoul (token, &end, 10);
    assert (end != token);
    assert (map_insert_data (S, e));
  }
  free (line);
  return map_size (S);
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  static char *formatter = "%zu";

  wchar_t *line = 0;
  assert (fgetwcs (&line, 0, stdin));
  size_t n = wcstoul (line, 0, 10);
  free (line);

  map *A = map_create (0, MAP_GENERIC_CMP, &(size_t){ sizeof (size_t) }, 1);
  map_set_context (A, formatter);
  fprintf (stderr, "%zu\n", fill_set (A, stdin));

  map *B = map_create (0, MAP_GENERIC_CMP, &(size_t){ sizeof (size_t) }, 1);
  map_set_context (B, formatter);
  fprintf (stderr, "%zu\n", fill_set (B, stdin));

  map *C = map_create (0, MAP_GENERIC_CMP, &(size_t){ sizeof (size_t) }, 1);
  map_set_context (C, formatter);
  for (size_t i = 1; i <= n; i++) {
    size_t *e = malloc (sizeof (*e));
    *e = i;
    if (!map_insert_data (C, e))
      free (e);
  }
  fprintf (stderr, "%zu\n", map_size (C));

  //------ Union ------
  fprintf (stdout, "{ ");
  // map_traverse (A, fprint_elem, stdout, MAP_NOT_IN, B);
  // map_traverse (B, fprint_elem, stdout, 0, 0);
  map *U = map_create (0, MAP_GENERIC_CMP, &(size_t){ sizeof (size_t) }, 1);
  map_set_context (U, formatter);
  map_traverse (A, MAP_COPY_REF_TO, U, 0, 0);
  map_traverse (B, MAP_COPY_REF_TO, U, 0, 0);
  map_traverse (U, fprint_elem, stdout, 0, 0);
  map_traverse (U, MAP_REMOVE_ALL, 0, 0, 0);
  map_destroy (U);
  fprintf (stdout, "}\n");
  //------ Intersection ------
  fprintf (stdout, "{ ");
  map_traverse (A, fprint_elem, stdout, MAP_ALSO_IN, B);
  fprintf (stdout, "}\n");
  //------ A - B ------
  fprintf (stdout, "{ ");
  map_traverse (A, fprint_elem, stdout, MAP_NOT_IN, B);
  fprintf (stdout, "}\n");
  //------ B - A ------
  fprintf (stdout, "{ ");
  map_traverse (B, fprint_elem, stdout, MAP_NOT_IN, A);
  fprintf (stdout, "}\n");
  //------ Complement to A ------
  fprintf (stdout, "{ ");
  map_traverse (C, fprint_elem, stdout, MAP_NOT_IN, A);
  fprintf (stdout, "}\n");
  //------ Complement to A ------
  fprintf (stdout, "{ ");
  map_traverse (C, fprint_elem, stdout, MAP_NOT_IN, B);
  fprintf (stdout, "}\n");

  map_traverse (C, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (C);
  map_traverse (B, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (B);
  map_traverse (A, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (A);
}
