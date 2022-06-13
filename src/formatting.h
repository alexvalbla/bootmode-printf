#ifndef BM_FORMATTING_H
#define BM_FORMATTING_H


#include "print.h"
#include "output_utils.h"

// #define REMOVE_FORMATTING_FLAGS(f) (f &= 0xFF00);
#define REMOVE_FORMATTING_FLAGS(f) (f &= ~((1U << 11)-1));
// formatting flags:
#define FLAG_ALTF (1U << 0) // alternate form
#define FLAG_ZERO (1U << 1) // zero-padded
#define FLAG_LADJ (1U << 2) // left-adjusted
#define FLAG_WSPC (1U << 3) // white space
#define FLAG_SIGN (1U << 4) // sign
#define FLAG_UCAS (1U << 5) // upper case
#define FLAG_PREC (1U << 6) // precision
#define FLAG_WDTH (1U << 7) // field width
#define FLAG_FLT0 (1U << 8) // f.p. is 0.
#define FLAG_NEGV (1U << 9) // f.p. has negative value
#define FLAG_REM0 (1U << 10) // remove trailing zeros for f.p. conversion

// for bootmode_snprintf and bootmode_vsnprintf:
#define FLAG_LIMIT (1U << 11) // limit n for character output is set

void output_int(bm_output_ctxt *ctxt, bm_va_list ap);

void output_str(bm_output_ctxt *ctxt, bm_va_list ap);

void output_n(bm_output_ctxt *ctxt, bm_va_list ap);

void output_fp(bm_output_ctxt *ctxt, bm_va_list ap);


#endif // formatting.h
