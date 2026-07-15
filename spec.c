#include "rosalind.h"

static struct mm {
  wchar_t *aminoacid;
  double mass;
} monoisotopic_mass_table[] = {
  { L"A", 71.03711 },
  { L"C", 103.00919 },
  { L"D", 115.02694 },
  { L"E", 129.04259 },
  { L"F", 147.06841 },
  { L"G", 57.02146 },
  { L"H", 137.05891 },
  { L"I", 113.08406 },
  { L"K", 128.09496 },
  { L"L", 113.08406 },
  { L"M", 131.04049 },
  { L"N", 114.04293 },
  { L"P", 97.05276 },
  { L"Q", 128.05858 },
  { L"R", 156.10111 },
  { L"S", 87.03203 },
  { L"T", 101.04768 },
  { L"V", 99.06841 },
  { L"W", 186.07931 },
  { L"Y", 163.06333 },
};

static const void *
get_mass (void *data) {
  return &(((struct mm *)data)->mass);
}

static int
dbl_cmp (const void *a, const void *b, const void *eps) {
  const double da = *(const double *)a;
  const double db = *(const double *)b;
  const double deps = *(const double *)eps;
  return da < db - deps ? -1 : (da > db + deps ? 1 : 0);
}

struct find_aa_arg {
  map *mmm;
  size_t length;
  double prev;
};

static int
find_aa (void *data, void *op_arg, int *, const void *) {
  struct find_aa_arg *arg = op_arg;
  struct mm *m;
  if (arg->length) {
    double diff = *((double *)data) - arg->prev;
    if (map_find_key (arg->mmm, &diff, MAP_GET_ONE, &m, 0, 0))
      fprintf (stdout, "%ls", m->aminoacid);
  }
  arg->length++;
  arg->prev = *(double *)data;
  return 1;
}

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");
  double epsilon = .0001;

  map *mmm = map_create (get_mass, dbl_cmp, &epsilon, 1);
  for (size_t i = 0; i < lengthof (monoisotopic_mass_table); i++)
    assert (map_insert_data (mmm, monoisotopic_mass_table + i));

  map *prefix_spectrum = map_create (0, dbl_cmp, &epsilon, 1);
  wchar_t *line = 0;
  while (fgetwcs (&line, 0, stdin)) {
    double *v = malloc (sizeof (*v));
    wchar_t *end;
    *v = wcstod (line, &end);
    assert (end != line);
    assert (map_insert_data (prefix_spectrum, v));
  }
  free (line);

  map_traverse (prefix_spectrum, find_aa, &(struct find_aa_arg){ .mmm = mmm, .length = 0 }, 0, 0);

  map_traverse (prefix_spectrum, MAP_REMOVE_ALL, free, 0, 0);
  map_destroy (prefix_spectrum);
  map_traverse (mmm, MAP_REMOVE_ALL, 0, 0, 0);
  map_destroy (mmm);
}
