#include "map.h"
#include "rosalind.h"
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int
main (void) {
  setlocale (LC_ALL, "");
  wchar_t *wcs1 = 0;
  wchar_t *wcs2 = 0;
  fgetwcs (&wcs1, 0, stdin);
  fgetwcs (&wcs2, 0, stdin);
  size_t hammer = 0;
  for (size_t i = 0; i < wcslen (wcs1); i++)
    hammer += wcs1[i] != wcs2[i] ? 1 : 0;
  fprintf (stdout, "%zu\n", hammer);
}
