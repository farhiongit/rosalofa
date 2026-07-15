#ifndef __FGETWTOK_H__
#define __FGETWTOK_H__
#include <stddef.h>
#include <stdio.h>

#ifndef UINTBIG_WIDTH
#define UINTBIG_WIDTH 19456 // 1024
#endif
typedef unsigned _BitInt (UINTBIG_WIDTH) uintbig_t;
#define UINTBIG_MAX ((uintbig_t)~0)

// A call to this function prepares fprintf family of functions for formatted output to stream.
// Use conversion specifier U for an unsigned big integer, V for a signed big integer. Flag characters + and ' are supported.
// E.g.                     : printf ("%+'V ; %+'V\n", INTBIG_MIN, INTBIG_MAX);
#define PRIUINTBIG "U"
void xintbig_printf_init (void);

// Similar to wcschr but the terminating null byte is NOT considered part of the string wca by default if not explicitly included (tokens can contain the null terminator.)
const wchar_t *wcinwcs (const wchar_t wca[], size_t wcalen, wchar_t wc);

// Read tokens (separated by characters of wca) from a stream, and run f for each of them. tokens are truncated if their length exceeds maxlen. Tokens are terminated with null.
size_t freadtok (FILE *stream, size_t maxlen, const wchar_t wca[], size_t wcalen, int (*f) (size_t linenum, size_t toknum, const wchar_t *token, size_t tokenlen, void *context), void *context);

// Similar to fgetws. Read a line from a stream (truncated at maxlen if not 0, terminated with null).
// It reads a string into the dynamically allocated wide-character array pointed to by *wcs, and adds a terminating null wide character (L'\0').
// It stops reading wide characters after it has encountered and stored a newline wide character.  It also stops when end of stream is reached.
// /!\ The "\n" is INCLUDED if it is read.
// /!\ *wcs must be set to 0 before the first call to fgetwcs and must only be free'd after the very last call to fgetwcs.
const wchar_t *fgetwcs (wchar_t **wcs, size_t maxlen, FILE *stream);

// Read a FASTA from stream. Free *name and *genetic after use.
int freadFASTA (wchar_t **name, wchar_t **genetic, FILE *stream);

#endif
