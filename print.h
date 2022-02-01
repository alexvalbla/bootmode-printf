#ifndef LIBPRINTF
#define LIBPRINTF


int bootmode_vsnprintf(char *str, size_t size, const char *format, va_list ap);

int bootmode_vsprintf(char *str, const char *format, va_list ap);

int bootmode_vprintf(const char *format, va_list ap);

int bootmode_snprintf(char *str, size_t size, const char *format, ...);

int bootmode_sprintf(char *str, const char *format, ...);

int bootmode_printf(const char *format, ...);


#endif
