#ifndef BM_FORMATTING_H
#define BM_FORMATTING_H


#include "print.h"
#include "output_utils.h"


#define FLAG_ALTF 0001 // alternate form
#define FLAG_ZERO 0002 // zero-padded
#define FLAG_LADJ 0004 // left-adjusted
#define FLAG_WSPC 0010 // white space
#define FLAG_SIGN 0020 // sign
#define FLAG_UCAS 0040 // upper case
#define FLAG_PREC 0100 // precision
#define FLAG_WDTH 0200 // field width

#define FLAG_LIMIT 0400 // limit n for character output


void output_d(bm_output_ctxt *ctxt, bm_va_list ap);

int output_u(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int output_x(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int output_o(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int output_p(bm_va_list ap, char *str, uint16_t prec, uint8_t flags);

int output_c(bm_va_list ap, char *str);

int output_s(bm_va_list ap, char *str, uint16_t prec, uint8_t flags);

void output_n(bm_va_list ap, char mods[2], ssize_t total);

int output_e(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int output_f(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int output_g(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);


void pad_conversion(char fmt, char *str, uint8_t flags, uint16_t length, uint16_t field_width);


#endif // formatting.h
