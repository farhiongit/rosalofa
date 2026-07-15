#include "rosalind.h"

int
main (void) {
  setlocale (LC_ALL, "");

  wchar_t *string = 0;
  fgetwcs (&string, 0, stdin);
  assert (string && *string);
  if (string[wcslen (string) - 1] == L'\n')
    string[wcslen (string) - 1] = L'\0';
  fprintf (stderr, "%ls\n", string);

  wchar_t *pattern = 0;
  fgetwcs (&pattern, 0, stdin);
  assert (pattern && *pattern);
  if (pattern[wcslen (pattern) - 1] == L'\n')
    pattern[wcslen (pattern) - 1] = L'\0';
  fprintf (stderr, "%ls\n", pattern);

  wchar_t *start;
  for (wchar_t *searchfrom = string; (start = wcsstr (searchfrom, pattern)); searchfrom = start + 1)
    fprintf (stdout, "%ti ", start - string + 1);

  fprintf (stdout, "\n");
  free (pattern);
  free (string);
}
