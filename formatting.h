#ifndef FORMATTING
#define FORMATTING

#include "conversion.h"

#define FLAG_ALTF 0001 //alternate form
#define FLAG_ZPAD 0002 //zero-padded
#define FLAG_LADJ 0004 //left-adjusted
#define FLAG_WSPC 0010 //white space
#define FLAG_SIGN 0020 //sign
#define FLAG_UCAS 0040 //upper case
#define FLAG_PREC 0100 //precision
#define FLAG_WDTH 0200 //field width


void str_rev(char *s, size_t sz);

void PUTS(char *s);


int convert_d(int64_t a, char *str, uint8_t flags);

int convert_u(uint64_t a, char *str, uint8_t flags);

int convert_x(uint64_t a, char *str, uint16_t prec, uint8_t flags);

int convert_o(uint64_t a, char *str, uint16_t prec, uint8_t flags);

void convert_n(va_list ap, char mods[2], ssize_t total);


int convert_e(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_f(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_g(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);


int fp_special_case(fpclass_t class, char *str);

int fp_fmt_e(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags);

int fp_fmt_f(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags);

int fp_fmt_g(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags);

#endif
