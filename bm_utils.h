#ifndef BM_UTILS_H
#define BM_UTILS_H


#include <stdint.h>
#include <limits.h>


// check for (unsigned) 128-bit integers:
#ifndef __SIZEOF_INT128__
        #error "__uint128_t not defined"
#endif


// defining bm_putchar:
#include <stdio.h>
#define bm_putchar putchar


// defining bm_alloca:
#include <alloca.h>
#define bm_alloca alloca


// defining bm_va_list functions
#include <stdarg.h>
#define bm_va_list va_list
#define bm_va_start va_start
#define bm_va_end va_end
#define bm_va_arg va_arg
#define bm_va_copy va_copy


#endif // bm_utils.h
