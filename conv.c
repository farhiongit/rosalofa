#include "rosalind.h"

static int
dbl_cmp (const void *a, const void *b, const void *eps) {
  const double da = *(const double *)a;
  const double db = *(const double *)b;
  const double deps = *(const double *)eps;
  return da < db - deps ? -1 : (da > db + deps ? 1 : 0);
}

static size_t
fill_set (map *S, FILE *f) {
  wchar_t *line = 0;
  assert (fgetwcs (&line, 0, f));
  wchar_t *token;
  for (wchar_t *state = line; (token = wcstok (0, L" \t\n", &state));) {
    double *e = malloc (sizeof (*e));
    wchar_t *end;
    *e = wcstod (token, &end);
    assert (end != token);
    assert (map_insert_data (S, e));
  }
  free (line);
  return map_size (S);
}

static double
substract (double a, double b) {
  return a - b;
}

struct operation_arg {
  double (*op) (double, double);
  double with;
  map *into;
};

static int
apply_operator (void *data, void *op_arg, int *remove, const void *context) {
  (void)remove;
  (void)context;
  struct operation_arg *operation_arg = op_arg;
  double *result = malloc (sizeof (*result));
  *result = operation_arg->op (operation_arg->with, *(double *)data);
  if (!map_insert_data (operation_arg->into, result))
    free (result);
  return 1;
}

struct conv_arg {
  double (*op) (double, double);
  map *with;
  map *into;
};

static int
convolute (void *data, void *op_arg, int *remove, const void *context) {
  (void)remove;
  (void)context;
  struct conv_arg *conv_arg = op_arg;
  map_traverse (conv_arg->with, apply_operator, &(struct operation_arg){ .op = conv_arg->op, .with = *(double *)data, .into = conv_arg->into }, 0, 0);
  return 1;
}

struct largest_multiplicity {
  size_t nb_occurrences;
  double value;
};

static void
get_largest_multiplicity (const void *key, size_t nb_occurrences, void *op_arg, void *context) {
  (void)context;
  struct largest_multiplicity *largest_multiplicity = op_arg;
  if (nb_occurrences > largest_multiplicity->nb_occurrences) {
    largest_multiplicity->nb_occurrences = nb_occurrences;
    largest_multiplicity->value = *(const double *)key;
  }
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  double epsilon = .0001;

  map *S1 = map_create (0, dbl_cmp, &epsilon, 0); // multi-set
  fill_set (S1, stdin);
  map *S2 = map_create (0, dbl_cmp, &epsilon, 0); // multi-set
  fill_set (S2, stdin);

  map *convolution = map_create (0, dbl_cmp, &epsilon, 0); // multi-set
  map_traverse (S1, convolute, &(struct conv_arg){ .op = substract, .with = S2, .into = convolution }, 0, 0);

  struct largest_multiplicity largest_multiplicity = { 0 };
  map_traverse_keys (convolution, get_largest_multiplicity, &largest_multiplicity);
  fprintf (stdout, "%zu\n%.5f\n", largest_multiplicity.nb_occurrences, fabs (largest_multiplicity.value));

  map_traverse (convolution, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (convolution);

  map_traverse (S2, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (S2);
  map_traverse (S1, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (S1);
}
