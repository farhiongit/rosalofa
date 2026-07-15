#include <locale.h>
#include <stdio.h>
#include <wchar.h>

int
main (void) {
  setlocale (LC_ALL, "");
  for (wint_t wc = 0; (wc = fgetwc (stdin)) != WEOF;)
    fprintf (stdout, "%lc", wc == L'T' ? L'U' : wc);
  fprintf (stdout, "\n");
}
