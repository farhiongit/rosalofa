#include "rosalind.h"

struct node {
  long value;
  size_t nb_from, nb_to;
  struct node **from, **to;
  size_t path_length;
  struct node *path;
};

static int
lt (long a, long b) {
  return a < b;
}

static int
gt (long a, long b) {
  return a > b;
}

static void
init_shortest_longest_path (struct node *node) {
  if (!node->nb_to || node->path_length)
    return;

  for (size_t i = 0; i < node->nb_to; i++)
    if (node->to[i]->nb_to)
      return;

  node->path = node->to[0];
  node->path_length = 1;
}

static void
compute_longest_path (struct node *node) {
  if (node->path_length)
    return;

  size_t max_length = 0;
  for (size_t i = 0; i < node->nb_to; i++) {
    compute_longest_path (node->to[i]);
    if (node->to[i]->path_length > max_length) {
      node->path = node->to[i];
      node->path_length = 1 + (max_length = node->to[i]->path_length);
    }
  }
}

static void
get_lgis (size_t la, long *a, int (*cmp) (long, long)) {
  struct node *dag_nodes = 0;
  dag_nodes = calloc (la, sizeof (*dag_nodes));
  for (size_t i = 0; i < la; i++)
    dag_nodes[i] = (struct node){ .value = a[i] };

  for (size_t i = 0; i < la; i++)
    for (size_t j = i + 1; j < la; j++)
      if (cmp (dag_nodes[i].value, dag_nodes[j].value)) {
        dag_nodes[i].to = realloc (dag_nodes[i].to, ++dag_nodes[i].nb_to * sizeof (*dag_nodes[i].to));
        dag_nodes[i].to[dag_nodes[i].nb_to - 1] = &dag_nodes[j];
        dag_nodes[j].from = realloc (dag_nodes[j].from, ++dag_nodes[j].nb_from * sizeof (*dag_nodes[j].from));
        dag_nodes[j].from[dag_nodes[j].nb_from - 1] = &dag_nodes[i];
      }

  for (size_t i = 0; i < la; i++)
    if (dag_nodes[i].nb_to == 0)
      for (size_t j = 0; j < dag_nodes[i].nb_from; j++)
        init_shortest_longest_path (dag_nodes[i].from[j]);

  struct node *lgis_start = 0;
  size_t lgis = 0;
  for (size_t i = 0; i < la; i++)
    if (dag_nodes[i].nb_from == 0) {
      compute_longest_path (dag_nodes + i);
      if (dag_nodes[i].path_length > lgis)
        lgis = (lgis_start = dag_nodes + i)->path_length;
    }

  for (; lgis_start; lgis_start = lgis_start->path)
    fprintf (stdout, "%li ", lgis_start->value);
  fprintf (stdout, "\n");

  for (size_t i = 0; i < la; i++) {
    free (dag_nodes[i].from);
    free (dag_nodes[i].to);
  }
  free (dag_nodes);
}

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *line = 0;
  fgetwcs (&line, 0, stdin);
  size_t n = wcstoul (line, 0, 0);
  free (line);

  line = 0;
  fgetwcs (&line, 0, stdin);
  wchar_t *tok;
  size_t la = 0;
  long *a = 0;
  for (wchar_t *ptr = line; (tok = wcstok (0, L" \t\n", &ptr));) {
    long v = wcstol (tok, 0, 0);
    a = realloc (a, ++la * sizeof (*a));
    a[la - 1] = v;
  }
  free (line);
  assert (la == n);

  get_lgis (la, a, lt);
  get_lgis (la, a, gt);

  free (a);
}
