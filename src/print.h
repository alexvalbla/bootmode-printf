#ifndef BM_LIBPRINTF_H
#define BM_LIBPRINTF_H


// defining integer types
#include <stdint.h>
// check for (unsigned) 128-bit integers:
#ifndef __SIZEOF_INT128__
        #error "__uint128_t not defined"
#endif


// is endianness of integers and floating points the same ?
// -> important for decomposition of floating points

#define IS_BIG_ENDIAN_INT (*(uint16_t *)"\0\xFF" < 0x100)
#define IS_BIG_ENDIAN_FLT (*(float *)"\xFF\0\0\0" < *(float *)"\0\0\0\xFF")
#define ARE_SAME_ENDIANNESS_INT_FLT (IS_BIG_ENDIAN_INT == IS_BIG_ENDIAN_FLT)

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

// error codes for following functions:
#define STROUT_NUL (-1)
#define FORMAT_NUL (-2)
#define FORMAT_ERR (-3)

int bootmode_vsnprintf(char *str, size_t size, const char *format, bm_va_list ap);

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap);

int bootmode_vprintf(const char *format, bm_va_list ap);

int bootmode_snprintf(char *str, size_t size, const char *format, ...);

int bootmode_sprintf(char *str, const char *format, ...);

int bootmode_printf(const char *format, ...);


#endif // print.h
