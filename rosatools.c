#include "rosatools.h"
//-------------------------------------------------------------------
#include <errno.h>
#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define lengthof(array) (sizeof (array) / sizeof ((array)[0]))

const wchar_t *
wcinwcs (const wchar_t wca[], size_t wcalen, wchar_t wc) {
  for (size_t i = 0; i < wcalen; i++)
    if (wc == wca[i])
      return wca + i;
  return 0;
}

size_t
freadtok (FILE *stream, size_t maxlen,
          const wchar_t wca[], size_t wcalen,
          int (*f) (size_t linenum, size_t toknum, const wchar_t *token, size_t tokenlen, void *context), void *context) {
  if (!wca || !wcalen) {
    wca = L" \t\n";
    wcalen = 3;
  }
  if (!maxlen)
    maxlen = 10 * 1000 * 1000;
  size_t linenum = 1;
  size_t toknum = 0;
  wchar_t *token = 0;
  size_t bufferlen = 0;
  wchar_t *writehead = 0;
  size_t nbtok = 0;
  for (wint_t wc; (wc = fgetwc (stream));) {
    if (wc == WEOF || wcinwcs (wca, wcalen, (wchar_t)wc)) {
      if ((writehead && token && ++nbtok && f && !f (linenum, ++toknum, token, (size_t)(writehead - token), context)) || wc == WEOF)
        break;
      if (wc == L'\n') {
        linenum++;
        toknum = 0;
      }
      writehead = 0;
    } else {
      if (!token && !(token = realloc (token, ((bufferlen = 1) + 1) * sizeof (*token))))
        break;
      if (!writehead)
        writehead = token;
      if (writehead >= token + bufferlen && bufferlen < maxlen) {
        ptrdiff_t delta = writehead - token;
        bufferlen *= 2;
        if (bufferlen > maxlen)
          bufferlen = maxlen;
        if (!(token = realloc (token, (bufferlen + 1) * sizeof (*token))))
          break;
        writehead = token + delta;
      }
      if (writehead < token + bufferlen) {
        *(writehead++) = (wchar_t)wc;
        *writehead = L'\0';
      }
    }
  }
  free (token);
  return nbtok;
}

const wchar_t *
fgetwcs (wchar_t **wcs, size_t maxlen, FILE *stream) {
  static size_t wcsincr = 100;
  size_t wcslength = wcsincr;
  if (!maxlen)
    maxlen = 10 * 1000 * 1000;
  if (wcslength > maxlen)
    wcslength = maxlen;
  if (!(*wcs = realloc (*wcs, (wcslength + 1) * sizeof (**wcs))))
    return 0;
  **wcs = L'\0';
  wchar_t *ret = 0;
  for (wchar_t *ptr = *wcs; (ptr = fgetws (ptr, (int)wcsincr + 1, stream));) {
    size_t l = wcslen (ptr);
    if (ptr[l - 1] == L'\n' || wcslength == maxlen) {
      ret = *wcs;
      break;
    }
    wcsincr *= 2;
    if (!(ret = *wcs = realloc (*wcs, (wcslength + wcsincr + 1) * sizeof (**wcs))))
      break;
    ptr = ret + wcslength;
    wcslength += wcsincr;
    if (wcslength > maxlen)
      wcslength = maxlen;
  }
  wcsincr = wcslength;
  return ret;
}

int
freadFASTA (wchar_t **name, wchar_t **genetic, FILE *stream) {
  wint_t wc;
  const wchar_t *sep;
  if (name)
    *name = 0;
  if (genetic)
    *genetic = 0;
  if ((wc = fgetwc (stream)) == WEOF || wc != L'>') {
    if (wc != WEOF)
      ungetwc (wc, stream);
    return 0;
  }
  for (size_t l = 0; (wc = fgetwc (stream)) != WEOF && !(sep = wcinwcs (L" \t\n", 3, (wchar_t)wc)); l++)
    if (name && (*name = realloc (*name, (l + 2) * sizeof (**name)))) {
      (*name)[l] = (wchar_t)wc;
      (*name)[l + 1] = L'\0';
    }
  if (name && !*name)
    return 0;
  if (wc == WEOF)
    return 0;

  ungetwc (wc, stream);
  while ((wc = fgetwc (stream)) != WEOF && wc != L'\n')
    ;

  for (size_t l = 0; (wc = fgetwc (stream)) != WEOF && wc != L'>';)
    if (wc != L'\n')
      if (genetic && (*genetic = realloc (*genetic, (l + 2) * sizeof (**genetic)))) {
        (*genetic)[l] = (wchar_t)wc;
        (*genetic)[l + 1] = L'\0';
        l++;
      }
  if (genetic && !*genetic)
    return 0;
  if (wc != WEOF)
    ungetwc (wc, stream);
  return 1;
}

//---------------------------------------------------
// printf extension (%V for intbig_t, %U for uintbig_t)
// see register_printf_specifier
// https://sourceware.org/bugzilla/attachment.cgi?id=3874&action=view
// http://www.gnu.org/software/libc/manual/html_node/Customizing-Printf.html#Customizing-Printf
static int PA_UINTBIG;
static wchar_t SPEC_UINTBIG = L'U';

#define DEFINE_PRINTER(name, printer)                                        \
  static int                                                                 \
  uintbig_to_##name (FILE *f, uintbig_t ubi, char flag) {                    \
    (void)f;                                                                 \
    (void)flag;                                                              \
    char string[UINTBIG_WIDTH <= 1 ? 2 : UINTBIG_WIDTH];                     \
    size_t length = 0;                                                       \
    for (uintbig_t a = ubi; a && length < lengthof (string) - 1; a = a / 10) \
      length++;                                                              \
    for (size_t i = 0; i < length; i++, ubi = ubi / 10)                      \
      string[length - i - 1] = (char)((ubi % 10) + '0');                     \
    string[length] = 0;                                                      \
    return printer (f, "%s", *string ? string : "0");                        \
  }                                                                          \
  struct __useless_struct_##name##_IMPL

// The macro DEFINE_PRINTER avoids the use of a pointer to a variadic function fsinkf (file, ...) = snprinf (0, 0, ...)
DEFINE_PRINTER (stream, fprintf);
#define FSINKF(file, ...) snprintf (0, 0, __VA_ARGS__)
DEFINE_PRINTER (sink, FSINKF);

static void
uintbig_va (void *mem, va_list *ap) {
  uintbig_t v = va_arg (*ap, uintbig_t);
  memcpy (mem, &v, sizeof (v));
}

static int
uintbig_printf (FILE *stream, const struct printf_info *info, const void *const args[]) {
  // info->spec == SPEC_UINTBIG
  uintbig_t a;
  memcpy (&a, *((void *const *const *)args)[0], sizeof (a)); // uh ! again
  int ret = uintbig_to_sink (0, a, info->group ? '\'' : 0);
  char *str = calloc ((size_t)(ret + 1), sizeof (*str));
  FILE *buffer = fmemopen (str, (size_t)(ret + 1), "w");
  uintbig_to_stream (buffer, a, info->group ? '\'' : 0);
  fclose (buffer);
  ret = fprintf (stream, "%*s", (info->left ? -info->width : info->width), str);
  free (str);
  return ret;
}

static int
uintbig_print_arginfo_size (const struct printf_info *info, size_t n, int *argtypes, int *size) {
  (void)info;
  /* We always take exactly one argument and this is big integer */
  if (n > 0)
    argtypes[0] = PA_UINTBIG;
  *size = (int)sizeof (uintbig_t);
  return 1;
}

void
xintbig_printf_init (void) {
  static int XINTBIG_PRINTF_INIT_DONE;

  if (XINTBIG_PRINTF_INIT_DONE)
    return;
  XINTBIG_PRINTF_INIT_DONE = 1;
  PA_UINTBIG = register_printf_type (uintbig_va);
  register_printf_specifier (SPEC_UINTBIG, uintbig_printf, uintbig_print_arginfo_size);
}
#ifdef TU
#include <locale.h>
static int
f (size_t linenum, size_t toknum, const wchar_t *token, size_t toklen, void *context) {
  (void)context;
  // fprintf (stdout, "(%zu;%zu): '%ls'[%zu]\n", linenum, toknum, token, toklen);
  fprintf (stdout, "(%zu;%zu): '", linenum, toknum);
  for (size_t i = 0; i < toklen; i++)
    fprintf (stdout, "%lc", (wint_t)token[i]);
  fprintf (stdout, "' [%zu]\n", toklen);
  return 1;
}

int
main (void) {
  setlocale (LC_ALL, "");
  xintbig_printf_init ();
  fprintf (stdout, "%'U\n", UINTBIG_MAX);
  fprintf (stdout, "%zu\n", freadtok (stdin, 0, L" \t\n", 3, f, 0));
}
#endif
