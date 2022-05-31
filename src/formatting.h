#ifndef BM_FORMATTING_H
#define BM_FORMATTING_H


#include "print.h"
#include "output_utils.h"

#define REMOVE_FORMATTING_FLAGS(f) (f &= 0xFF00);
// formatting flags:
#define FLAG_ALTF 0x01 // alternate form
#define FLAG_ZERO 0x02 // zero-padded
#define FLAG_LADJ 0x04 // left-adjusted
#define FLAG_WSPC 0x08 // white space
#define FLAG_SIGN 0x10 // sign
#define FLAG_UCAS 0x20 // upper case
#define FLAG_PREC 0x40 // precision
#define FLAG_WDTH 0x80 // field width

// for bootmode_snprintf and bootmode_vsnprintf:
#define FLAG_LIMIT 0x100 // limit n for character output is set


void output_d(bm_output_ctxt *ctxt, bm_va_list ap);

void output_u(bm_output_ctxt *ctxt, bm_va_list ap);

void output_x(bm_output_ctxt *ctxt, bm_va_list ap);

void output_o(bm_output_ctxt *ctxt, bm_va_list ap);

void output_p(bm_output_ctxt *ctxt, bm_va_list ap);

void output_c(bm_output_ctxt *ctxt, bm_va_list ap);

void output_s(bm_output_ctxt *ctxt, bm_va_list ap);

void output_n(bm_output_ctxt *ctxt, bm_va_list ap);

void output_fp(bm_output_ctxt *ctxt, bm_va_list ap);


#endif // formatting.h
