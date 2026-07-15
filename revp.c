#include "rosalind.h"

struct revp {
  wchar_t *begin, *end;
};

struct queue_elem {
  struct revp value;
  struct queue_elem *next;
};

struct queue {
  struct queue_elem *head, *end;
};

static wchar_t
getdnacomplement (wchar_t letter) {
  switch (letter) {
  case L'A':
    return L'T';
  case L'T':
    return L'A';
  case L'C':
    return L'G';
  case L'G':
    return L'C';
  default:
    exit (EXIT_FAILURE);
  }
}

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *dna = 0;
  if (!freadFASTA (0, &dna, stdin))
    return EXIT_FAILURE;

  struct queue q = { 0 };
  struct queue_elem *new;

  for (wchar_t *p = dna + 1; *p; p++)
    if (*(p - 1) == getdnacomplement (*p))
      if ((new = malloc (sizeof (*new)))) {
        new->value.begin = p - 1;
        new->value.end = p;
        new->next = 0;
        *(!q.head ? &q.head : &q.end->next) = new;
        q.end = new;
      }

  struct queue_elem *next;
  for (struct queue_elem *e = q.head; e; e = next) {
    if (e->value.begin - 1 >= dna && *(e->value.end + 1) && *(e->value.begin - 1) == getdnacomplement (*(e->value.end + 1)))
      if ((new = malloc (sizeof (*new)))) {
        new->value.begin = e->value.begin - 1;
        new->value.end = e->value.end + 1;
        new->next = 0;
        q.end->next = new;
        q.end = new;
        fprintf (e->value.end - e->value.begin + 1 > 12 ? stderr : stdout, "%ti %ti\n", new->value.begin - dna + 1, new->value.end - new->value.begin + 1);
      }
    next = e->next;
    free (e);
  }

  free (dna);
}
