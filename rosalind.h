#undef NDEBUG // for assert
// #define __STDC_IEC_60559_DFP__ // for _Decimal
#include "aho_corasick.h"
#include "map.h"
#include "newick.h"
#include "rosatools.h"
#include "suffixtree.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define lengthof(array) (sizeof (array) / sizeof ((array)[0]))
