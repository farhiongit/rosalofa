#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();
  wchar_t *genetic = 0;
  freadFASTA (0, &genetic, stdin);
  uintbig_t nA = 0;
  uintbig_t nC = 0;
  for (wchar_t *p = genetic; *p; p++)
    if (*p == L'A')
      nA++;
    else if (*p == L'C')
      nC++;
  free (genetic);
  uintbig_t t = 1;
  for (; nA; t *= nA--)
    ;
  for (; nC; t *= nC--)
    ;
  fprintf (stdout, "%U", t);
}
