#include "rosalind.h"
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct _s_path {
  wchar_t *end1;
  wchar_t *end2;
  size_t distance;
} path_t;

// Run with LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975
int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *s1 = 0;
  assert (freadFASTA (0, &s1, stdin));
  wchar_t *s2 = 0;
  assert (freadFASTA (0, &s2, stdin));

  size_t *edit_grid = calloc ((wcslen (s1) + 1) * (wcslen (s2) + 1), sizeof (*edit_grid));

#ifdef DISCOVER_STRATEGY
  for (size_t i = 1; i < (wcslen (s1) + 1) * (wcslen (s2) + 1); i++)
    edit_grid[i] = wcslen (s1) + wcslen (s2);
  map *edit_paths = map_create (0, 0, 0, 0); // queue
  path_t *start = malloc (sizeof (*start));
  *start = (path_t){ .distance = 0, .end1 = s1, .end2 = s2 };
  assert (map_insert_data (edit_paths, start));

  for (void *data; map_traverse (edit_paths, MAP_REMOVE_ONE, &data, 0, 0); free (data)) {
    path_t *path = data;
    // fprintf (stderr, "(%zu, %zu) = %zu\n", (size_t)(path->end1 - s1), (size_t)(path->end2 - s2), path->distance);
    if (*path->end1
        && path->distance + 1 < edit_grid[(wcslen (s1) + 1) * (size_t)(path->end2 - s2) + (size_t)(path->end1 + 1 - s1)]) {
      path_t *new = malloc (sizeof (*new));
      *new = (path_t){ .distance = path->distance + 1, .end1 = path->end1 + 1, .end2 = path->end2 };
      assert (map_insert_data (edit_paths, new));
      edit_grid[(wcslen (s1) + 1) * (size_t)(path->end2 - s2) + (size_t)(path->end1 + 1 - s1)] = path->distance + 1;
    }
    if (*path->end2
        && path->distance + 1 < edit_grid[(wcslen (s1) + 1) * (size_t)(path->end2 + 1 - s2) + (size_t)(path->end1 - s1)]) {
      path_t *new = malloc (sizeof (*new));
      *new = (path_t){ .distance = path->distance + 1, .end1 = path->end1, .end2 = path->end2 + 1 };
      assert (map_insert_data (edit_paths, new));
      edit_grid[(wcslen (s1) + 1) * (size_t)(path->end2 + 1 - s2) + (size_t)(path->end1 - s1)] = path->distance + 1;
    }
    if (*path->end1 && *path->end2
        && (path->distance + (*path->end1 != *path->end2)) < edit_grid[(wcslen (s1) + 1) * (size_t)(path->end2 + 1 - s2) + (size_t)(path->end1 + 1 - s1)]) {
      path_t *new = malloc (sizeof (*new));
      *new = (path_t){ .distance = path->distance + (*path->end1 != *path->end2), .end1 = path->end1 + 1, .end2 = path->end2 + 1 };
      assert (map_insert_data (edit_paths, new));
      edit_grid[(wcslen (s1) + 1) * (size_t)(path->end2 + 1 - s2) + (size_t)(path->end1 + 1 - s1)] = path->distance + (*path->end1 != *path->end2);
    }
  } // for (void *data; map_traverse ...
  map_destroy (edit_paths);
#else
  for (size_t i = 0; i <= wcslen (s1); i++)
    edit_grid[i] = i;
  for (size_t j = 0; j <= wcslen (s2); j++)
    edit_grid[(wcslen (s1) + 1) * j] = j;
  for (size_t i = 1; i <= wcslen (s1); i++)
    for (size_t j = 1; j <= wcslen (s2); j++)
      edit_grid[(wcslen (s1) + 1) * j + i] = min (min (edit_grid[(wcslen (s1) + 1) * j + (i - 1)] + 1, edit_grid[(wcslen (s1) + 1) * (j - 1) + i] + 1),
                                                  edit_grid[(wcslen (s1) + 1) * (j - 1) + (i - 1)] + (s1[i - 1] != s2[j - 1]));
#endif

  // Show grid.
  fprintf (stderr, "  ");
  for (size_t j = 0; j < wcslen (s2); j++)
    fprintf (stderr, "   %lc", (wint_t)s2[j]);
  fprintf (stderr, "\n");
  for (size_t i = 0; i < wcslen (s1); i++) {
    fprintf (stderr, "%lc ", (wint_t)s1[i]);
    for (size_t j = 0; j < wcslen (s2) + 1; j++)
      fprintf (stderr, " %3zu", edit_grid[(wcslen (s1) + 1) * j + i]);
    fprintf (stderr, "\n");
  }
  fprintf (stderr, "  ");
  for (size_t j = 0; j < wcslen (s2) + 1; j++)
    fprintf (stderr, " %3zu", edit_grid[(wcslen (s1) + 1) * j + wcslen (s1)]);
  fprintf (stderr, "\n");

  // Get distance
  size_t path_distance = edit_grid[(wcslen (s1) + 1) * wcslen (s2) + wcslen (s1)];
  fprintf (stdout, "%zu\n", path_distance);

  // Trace back one solution.
  wchar_t *s1prime = calloc (wcslen (s1) + wcslen (s2) + 1, sizeof (*s1prime));
  wchar_t *s2prime = calloc (wcslen (s1) + wcslen (s2) + 1, sizeof (*s2prime));
  edit_grid[0] = 0;
  size_t i = wcslen (s1);
  size_t j = wcslen (s2);
  size_t k = 0;
  while (i || j) {
    if (i && j && s1[i - 1] != s2[j - 1] && edit_grid[(wcslen (s1) + 1) * j + i] == edit_grid[(wcslen (s1) + 1) * (j - 1) + (i - 1)] + 1) {
      s1prime[k] = s1[--i];
      s2prime[k++] = s2[--j];
      path_distance--;
    } else if (i && j && s1[i - 1] == s2[j - 1] && edit_grid[(wcslen (s1) + 1) * j + i] == edit_grid[(wcslen (s1) + 1) * (j - 1) + (i - 1)]) {
      s1prime[k] = s1[--i];
      s2prime[k++] = s2[--j];
    } else if (i && edit_grid[(wcslen (s1) + 1) * j + i] == edit_grid[(wcslen (s1) + 1) * j + (i - 1)] + 1) {
      s1prime[k] = s1[--i];
      s2prime[k++] = L'-';
      path_distance--;
    } else if (j && edit_grid[(wcslen (s1) + 1) * j + i] == edit_grid[(wcslen (s1) + 1) * (j - 1) + i] + 1) {
      s1prime[k] = L'-';
      s2prime[k++] = s2[--j];
      path_distance--;
    } else
      break;
  }
  assert (!path_distance);
  for (i = k; i; i--)
    fprintf (stdout, "%lc", (wint_t)s1prime[i - 1]);
  fprintf (stdout, "\n");
  for (j = k; j; j--)
    fprintf (stdout, "%lc", (wint_t)s2prime[j - 1]);
  fprintf (stdout, "\n");
  free (s2prime);
  free (s1prime);

  free (edit_grid);
  free (s2);
  free (s1);
}
