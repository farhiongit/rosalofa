#undef NDEBUG
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static wchar_t *globalcopy;
static void
freeatexit (void) {
  fprintf (stderr, "global %ls\n", globalcopy);
  free (globalcopy);
}

static void
freeonexit (int ret, void *arg) {
  (void)ret;
  wchar_t *localcopy = *(wchar_t **)arg;
  fprintf (stderr, "local %ls\n", localcopy);
  free (localcopy);
}

int
main (void) {
  setlocale (LC_ALL, "");
  assert (!atexit (freeatexit));
  globalcopy = wcsdup (L"text");

  static wchar_t *localcopy;
  assert (!on_exit (freeonexit, &localcopy));
  localcopy = wcsdup (L"text");
}
