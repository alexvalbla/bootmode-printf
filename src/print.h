#ifndef BM_LIBPRINTF_H
#define BM_LIBPRINTF_H


// defining integer types
#include <stdint.h>
// check for (unsigned) 128-bit integers:
#ifndef __SIZEOF_INT128__
        #error "__uint128_t not defined"
#endif


// defining bm_putchar:
#include <stdio.h>
#define bm_putchar putchar


// defining bm_va_list functions
#include <stdarg.h>
#define bm_va_list va_list
#define bm_va_start va_start
#define bm_va_end va_end
#define bm_va_arg va_arg
#define bm_va_copy va_copy


int bootmode_vsnprintf(char *str, size_t size, const char *format, bm_va_list ap);

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap);

int bootmode_vprintf(const char *format, bm_va_list ap);

int bootmode_snprintf(char *str, size_t size, const char *format, ...);

int bootmode_sprintf(char *str, const char *format, ...);

int bootmode_printf(const char *format, ...);


#endif // print.h
