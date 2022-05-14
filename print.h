#ifndef BM_LIBPRINTF_H
#define BM_LIBPRINTF_H


#include "bm_utils.h"


int bootmode_vsnprintf(char *str, size_t size, const char *format, bm_va_list ap);

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap);

int bootmode_vprintf(const char *format, bm_va_list ap);

int bootmode_snprintf(char *str, size_t size, const char *format, ...);

int bootmode_sprintf(char *str, const char *format, ...);

int bootmode_printf(const char *format, ...);


#endif // print.h
