#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *s = 0;
  freadFASTA (0, &s, stdin);
  wchar_t *t = 0;
  freadFASTA (0, &t, stdin);

  wchar_t **pos = calloc (wcslen (t), sizeof (*pos));

  size_t i;
  for (i = 0; i < wcslen (t); i++)
    if (i == 0 && !((pos[i] = wcschr (s, t[i]))))
      break;
    else if (i != 0 && !((pos[i] = wcschr (pos[i - 1] + 1, t[i]))))
      break;

  if (i < wcslen (t))
    return EXIT_FAILURE;

  for (int go = 1; go;) {
    for (i = 0; i < wcslen (t); i++)
      fprintf (stdout, "%s%zu", i == 0 ? "" : " ", (size_t)(pos[i] + 1 - s));
    fprintf (stdout, "\n");

    for (i = wcslen (t); i; i--)
      if ((pos[i - 1] = wcschr (pos[i - 1] + 1, t[i - 1])))
        break;

    if (i) {
      for (; i < wcslen (t); i++)
        if (!(pos[i] = wcschr (pos[i - 1] + 1, t[i]))) {
          go = 0;
          break;
        }
    } else
      go = 0;
  }

  free (pos);
  free (t);
  free (s);
}
