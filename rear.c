#include "rosalind.h"

/*
1 2 3 4 5 6 7 8 9
3 1 5 2 7 4 9 6 8

1 2 3 4 5 6 7 8 9 0
3 1 5 2 7 4 9 6 0 8

0 1 2 3 4 5 6 7 8 9
4 3 0 7 6 5 1 2 8 9

8 6 7 9 4 1 3 0 2 5
8 2 7 6 9 1 5 3 0 4

3 9 0 4 1 8 6 7 5 2
2 9 8 5 1 7 3 4 6 0

1 2 3 4 5 6 7 8 9 0
1 2 3 4 5 6 7 8 9 0
-----
9 4 5 7 0
*/

static int
eq_elem (long a, long b) {
  return a == b ? 1 : 0;
}

static int
lt_elem (long a, long b) {
  return a < b ? 1 : 0;
}

static long
incr_elem (long a) {
  static const long one = 1;
  return a + one;
}

static int
adjacent_elem (long a, long b) {
  return (incr_elem (a) == b || a == incr_elem (b)) ? 1 : 0;
}

static long
id_perm (size_t i) {
  return (long)(i + 1);
}

static int
eq_perm (size_t l, long *a, long *b) {
  for (size_t k = 0; k < l; k++)
    if (!eq_elem (a[k], b[k]))
      return 0;
  return 1;
}

static int
cmp_perm (size_t l, long *a, long *b) {
  for (size_t k = 0; k < l; k++)
    if (lt_elem (a[k], b[k]))
      return -1;
    else if (lt_elem (b[k], a[k]))
      return 1;
  return 0;
}

struct invalid_perm {
  size_t depth;
  long *perm;
};

static void
invalid_perm_free (void *invalid_perm) {
  free (((struct invalid_perm *)invalid_perm)->perm);
  free (invalid_perm);
}

static int
invalid_perm_cmp (const void *a, const void *b, const void *arg) {
  const size_t l = (*(const size_t *)arg);
  const struct invalid_perm ipa = *(const struct invalid_perm *)a;
  const struct invalid_perm ipb = *(const struct invalid_perm *)b;
  if (ipa.depth < ipb.depth)
    return -1;
  else if (ipa.depth > ipb.depth)
    return 1;
  else
    return cmp_perm (l, ipa.perm, ipb.perm);
}

static void
permutation_normalize (size_t len, long *start, const long *end) {
  for (size_t i = 0; i < len; i++)
    for (size_t j = 0; j < len; j++)
      if (eq_elem (start[i], end[j])) {
        start[i] = id_perm (j);
        break;
      }
}

static size_t
nb_breakpoints (size_t l, const long *a) {
  size_t nb = 0;
  if (a[0] != id_perm (0))
    nb++;
  for (size_t i = 1; i < l; i++)
    if (!adjacent_elem (a[i], a[i - 1]))
      nb++;
  if (a[l - 1] != id_perm (l - 1))
    nb++;
  return nb;
}

static size_t
max_reversal_sort (size_t l, const long *a) {
  assert (l && a);
  size_t max = 0;
  long *atmp = malloc (l * sizeof (*atmp));
  for (size_t i = 0; i < l; i++)
    atmp[i] = a[i];
  for (size_t i = 0; i < l; i++)
    if (atmp[i] != id_perm (i))
      for (size_t j = i + 1; j < l; j++)
        if (eq_elem (atmp[j], id_perm (i))) {
          fprintf (stderr, "(%zu,%zu)", i + 1, j + 1);
          for (size_t k = i; k < j; k++, j--) {
            long tmp = atmp[k];
            atmp[k] = atmp[j];
            atmp[j] = tmp;
          }
          max++;
          break;
        }
  fprintf (stderr, "\n");
  for (size_t i = 0; i < l; i++)
    assert (eq_elem (atmp[i], id_perm (i)));
  free (atmp);
  assert (max < l);
  return max;
}

static size_t
min_distance (size_t l, const long *a) {
  assert (l && a);
  return nb_breakpoints (l, a) / 2;
}

static size_t
max_distance (size_t l, const long *a) {
  return max_reversal_sort (l, a);
}

static size_t
reversal_sort (const size_t max, size_t indent, const size_t len, long *gabuf, size_t *sol, size_t *nb_bp, map *invalid_perm, const int first_only, const int details) {
  assert (len && gabuf);
  long *abuf = gabuf + indent * len;
  size_t ret = ~(size_t)0;
  for (size_t i = 0; indent < max && i < len; i++)
    if (i == 0 || !adjacent_elem (abuf[i - 1], abuf[i]))
      for (size_t j = i + 1; j < len; j++)
        if (j == (len - 1) || !adjacent_elem (abuf[j], abuf[j + 1])) {
          if (indent && i + 1 == sol[(indent - 1) * 2 + 0] && j + 1 == sol[(indent - 1) * 2 + 1])
            continue;
          size_t k, l, rd;
          for (k = 0; k < len; k++)
            abuf[k + len] = abuf[k];
          for (k = i, l = j; k < l; k++, l--) {
            long tmp = abuf[k + len];
            abuf[k + len] = abuf[l + len];
            abuf[l + len] = tmp;
          } // for
          int cont = 0;
          for (k = indent; !cont && k > 0; k--)
            if (eq_perm (len, abuf, gabuf + (k - 1) * len))
              cont = 1;
          if (cont)
            continue;
          cont = 0;
          for (k = indent; !cont && k <= indent; k--)
            if (invalid_perm && map_find_key (invalid_perm, &(struct invalid_perm){ .depth = k, .perm = abuf + len }, MAP_EXISTS_ONE, 0, 0, 0))
              cont = 1;
          if (cont)
            continue;
          sol[indent * 2 + 0] = i + 1;
          sol[indent * 2 + 1] = j + 1;
          nb_bp[indent + 1] = nb_breakpoints (len, abuf + len);
          size_t min = nb_bp[indent + 1] / 2;
          if (min == 0) { // A solution was found
            static int first = 1;
            fprintf (stdout, "%s%zu", !first ? (!details ? " " : "\n") : "", indent + 1);
            fflush (stdout);
            first = 0;
            for (size_t step = 0; step <= indent; step++)
              if (details)
                fprintf (stdout, "\n%zu %zu", sol[2 * step], sol[2 * step + 1]);
              else
                fprintf (stderr, "(%zu,%zu)", sol[2 * step], sol[2 * step + 1]);
            fflush (stdout);
            if (!details)
              fprintf (stderr, "\n");
            return (ret = 1);
          } // if (min == 0)
          else if (min + indent <= max && (rd = reversal_sort (max, indent + 1, len, gabuf, sol, nb_bp, invalid_perm, first_only, details)) < ret)
            ret = rd + 1;
          else {
            if (invalid_perm) {
              struct invalid_perm *ip = malloc (sizeof (*ip));
              ip->depth = indent;
              ip->perm = malloc (len * sizeof (*ip->perm));
              for (l = 0; l < len; l++)
                ip->perm[l] = gabuf[l + (indent + 1) * len];
              if (!map_insert_data (invalid_perm, ip))
                invalid_perm_free (ip);
            }
            continue;
          }
          if (first_only)
            return ret;
        } // if (j == (len - 1) || !adjacent (abuf[j], abuf[j + 1]))
  return ret;
}

int
main (int argc, char **argv) {
  (void)argc;
  setlocale (LC_ALL, "");
  const int rear = !strcmp ("rear", strchr (*argv, '/') ? strrchr (*argv, '/') + 1 : *argv);
  for (size_t t = 0;; t++) {
    wchar_t *line1 = 0;
    wchar_t *line2 = 0;
    wchar_t *line3 = 0;

    fprintf (stderr, "-\n");
    line1 = 0;
    if (!fgetwcs (&line1, 0, stdin))
      break;
    line2 = 0;
    if (!fgetwcs (&line2, 0, stdin))
      break;

    wchar_t *tok;
    size_t la1 = 0;
    long *a1 = 0;
    for (wchar_t *ptr = line1; (tok = wcstok (0, L" \t\n", &ptr));) {
      long v = wcstol (tok, 0, 0);
      a1 = realloc (a1, ++la1 * sizeof (*a1));
      a1[la1 - 1] = v;
    }

    size_t la2 = 0;
    long *a2 = 0;
    for (wchar_t *ptr = line2; (tok = wcstok (0, L" \t\n", &ptr));) {
      long v = wcstol (tok, 0, 0);
      a2 = realloc (a2, ++la2 * sizeof (*a2));
      a2[la2 - 1] = v;
    }

    free (line2);
    free (line1);

    assert (la1 == la2);
    permutation_normalize (la1, a1, a2);
    free (a2);
    a2 = 0;

    if (la1) {
      size_t min = min_distance (la1, a1);
      size_t max = max_distance (la1, a1);
      fprintf (stderr, "\n%zu <= ? <= %zu < %zu\n", min, max, la1);
      assert (min <= max);
      assert (max < la1);

      size_t ret = 0;
      if (min) {
        long *abuf = malloc (la1 * la1 * sizeof (*abuf));
        size_t *sol = malloc (2 * (la1 - 1) * sizeof (*sol));
        size_t *nb_bp = malloc (la1 * sizeof (*nb_bp));
        for (size_t i = 0; i < la1; i++)
          abuf[i] = a1[i];
        nb_bp[0] = nb_breakpoints (la1, abuf);
        for (size_t lim = min; lim <= max; lim++) {
          map *invalid_perm = map_create (0, invalid_perm_cmp, &(size_t){ la1 }, 1);
          if ((ret = reversal_sort (lim, 0, la1, abuf, sol, nb_bp, invalid_perm, 1, rear == 0)) <= lim) {
            map_traverse (invalid_perm, MAP_REMOVE_ALL, invalid_perm_free, 0, 0);
            map_destroy (invalid_perm);
            break;
          }
          map_traverse (invalid_perm, MAP_REMOVE_ALL, invalid_perm_free, 0, 0);
          map_destroy (invalid_perm);
        }
        free (nb_bp);
        free (sol);
        free (abuf);
      } // if (min)
      else
        fprintf (stdout, "%s0", t ? (rear ? " " : "\n") : "");
      assert (ret < la1);
    } // if (la1)

    free (a1);

    line3 = 0;
    if (!fgetwcs (&line3, 0, stdin))
      break;

    free (line3);
  } // for (size_t t = 0;; t++)
  fprintf (stdout, "\n");
}
