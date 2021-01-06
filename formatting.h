#ifndef FORMATTING
#define FORMATTING

#include "conversion.h"

#define FLAG_ALTF 0001 //alternate form
#define FLAG_ZERO 0002 //zero-padded
#define FLAG_LADJ 0004 //left-adjusted
#define FLAG_WSPC 0010 //white space
#define FLAG_SIGN 0020 //sign
#define FLAG_UCAS 0040 //upper case
#define FLAG_PREC 0100 //precision
#define FLAG_WDTH 0200 //field width


int convert_d(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_u(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_x(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_o(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_p(va_list ap, char *str, uint16_t prec, uint8_t flags);

int convert_c(va_list ap, char *str);

int convert_s(va_list ap, char *str, uint16_t prec, uint8_t flags);

void convert_n(va_list ap, char mods[2], ssize_t total);

int convert_e(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_f(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);

int convert_g(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags);


void pad_conversion(char fmt, char *str, uint8_t flags, uint16_t length, uint16_t field_width);


#endif
