#ifndef FORMATTING
#define FORMATTING

#include "conversion.h"

typedef enum {SPACE_PADDED = 0, ZERO_PADDED, RIGHT_PADDED} P_mode;
//for padding in case of specified field length

typedef enum {NO_SIGN = 0, BLANK, SIGN} S_mode;
//sign flags

void str_rev(char *s, size_t sz);

void PUTS(char *s);

void string_s(char *s, char *str, unsigned int strIndex, P_mode Padding_F, unsigned int length, unsigned int padding_length);

int string_d(int64_t a, char *s);

int string_u(uint64_t a, char *s);

int string_x(uint64_t a, char *s, int mod);

int string_o(uint64_t a, char *s);


int fp_special_case(fpclass_t class, char *str);


int fp_fmt_e(char *str, unsigned int prec, int s, __uint128_t n, int32_t F);

int string_e(double f, char *str, unsigned int prec);

int string_Le(long double f, char *str, unsigned int prec);


int fp_fmt_f(char *str, unsigned int prec, int s, __uint128_t n, int32_t F);

int string_f(double f, char *s, unsigned int decimal);

int string_Lf(long double f, char *s, unsigned int decimal);


int fp_fmt_g(char *str, unsigned int prec, int s, __uint128_t n, int32_t F);

int string_g(double f, char *s, unsigned int decimal);

int string_Lg(long double f, char *s, unsigned int decimal);


#endif
