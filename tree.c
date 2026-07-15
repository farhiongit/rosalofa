#include "rosalind.h"

struct elem {
  long value;
  struct elem *next;
};

struct set {
  struct elem *elem;
  struct set *next;
};

static struct set *
getset (struct set *sets, long v) {
  for (; sets; sets = sets->next)
    for (struct elem *elem = sets->elem; elem; elem = elem->next)
      if (v == elem->value)
        return sets;
  return 0;
}

static int
inset (struct set *set, long v) {
  assert (set);
  for (struct elem *elem = set->elem; elem; elem = elem->next)
    if (v == elem->value)
      return 1;
  return 0;
}

static struct set *
addtoset (struct set *set, long v) {
  assert (set);
  if (!inset (set, v)) {
    struct elem *new = malloc (sizeof (*new));
    *new = (struct elem){ .value = v, .next = set->elem };
    set->elem = new;
  }
  return set;
}

static struct set *
newset (struct set *sets) {
  struct set *new = malloc (sizeof (*new));
  *new = (struct set){ 0 };
  if (sets)
    new->next = sets;
  return new;
}

int
main (void) {
  char line[100];
  assert (fgets (line, 100, stdin));
  size_t nb_elem = strtoul (strtok (line, " \t\n"), 0, 0);
  size_t nb_edges = nb_elem - 1;

  struct set *sets = 0;
  while (fgets (line, 100, stdin)) {
    long u = strtol (strtok (line, " \t\n"), 0, 0);
    long v = strtol (strtok (0, " \t\n"), 0, 0);
    struct set *set;
    if ((set = getset (sets, u)))
      addtoset (set, v);
    else if ((set = getset (sets, v)))
      addtoset (set, u);
    else
      addtoset (addtoset (sets = newset (sets), u), v);
    nb_edges--;
  }

  fprintf (stderr, "%lu\n", nb_elem);
  struct set *nexts;
  for (struct set *set = sets; set; set = nexts) {
    struct elem *nexte;
    for (struct elem *elem = set->elem; elem; elem = nexte) {
      fprintf (stderr, "%li ", elem->value);
      nexte = elem->next;
      free (elem);
      nb_elem--;
    }
    fprintf (stderr, "\n");
    nb_elem++;
    nexts = set->next;
    free (set);
  }
  fprintf (stderr, "%lu\n", nb_elem);
  fprintf (stderr, "\n");
  fprintf (stderr, "%lu\n", nb_elem);
  fprintf (stderr, "%lu\n", nb_elem - 1);
  fprintf (stdout, "%lu", nb_edges);
}
