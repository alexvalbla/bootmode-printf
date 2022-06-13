#include "print.h"
#include "formatting.h"


static int main_output_loop(bm_output_ctxt *ctxt, const char *format, bm_va_list ap) {
        if (format == NULL) {
                return FORMAT_NUL;
        }

        // main loop:
        size_t i = 0; // where we are on the format string
        while(format[i] != '\0') {
                if (format[i] != '%') {
                        // most common case: simply output the character
                        output_char(ctxt, format[i++]);
                } else {
                        // this is where the fun begins...
                        REMOVE_FORMATTING_FLAGS(ctxt->flags) // clean slate

                        // mark flags:
                        begin_flag_loop:
                        ++i;
                        switch (format[i]) {
                                case '#':
                                        ctxt->flags |= FLAG_ALTF;
                                        goto begin_flag_loop;
                                case '0':
                                        ctxt->flags |= FLAG_ZERO;
                                        goto begin_flag_loop;
                                case '-':
                                        ctxt->flags |= FLAG_LADJ;
                                        goto begin_flag_loop;
                                case ' ':
                                        ctxt->flags |= FLAG_WSPC;
                                        goto begin_flag_loop;
                                case '+':
                                        ctxt->flags |= FLAG_SIGN;
                                        goto begin_flag_loop;
                                default:
                                        break;
                        }

                        // field width modifier:
                        if ((format[i] >= '1' && format[i] <= '9') || format[i] == '*') {
                                ctxt->flags |= FLAG_WDTH;
                                int field_width;
                                if (format[i] == '*') {
                                        field_width = bm_va_arg(ap, int);
                                        ++i;
                                } else {
                                        field_width = format[i++] - '0';
                                        while (format[i] >= '0' && format[i] <= '9') {
                                                field_width = field_width*10 + (format[i++]-'0');
                                        }
                                }
                                if (field_width < 0) {
                                        // interpreted as - flag followed by a positive field width
                                        ctxt->flags |= FLAG_LADJ;
                                        field_width = -field_width;
                                }
                                ctxt->field_width = field_width;
                        }

                        if (ctxt->flags&FLAG_LADJ) {
                                // - flags overrides 0 flag
                                ctxt->flags &= ~((uint16_t)(FLAG_ZERO));
                        }

                        // precision modifier:
                        if (format[i] == '.') {
                                ctxt->flags |= FLAG_PREC;
                                int precision;
                                ++i;
                                if (format[i] == '*') {
                                        precision = bm_va_arg(ap, int);
                                        ++i;
                                } else {
                                        precision = 0;
                                        while(format[i] >= '0' && format[i] <= '9') {
                                                precision = precision*10 + (format[i++]-'0');
                                        }
                                }
                                if (precision < 0) {
                                        // interpreted as precision being omitted
                                        ctxt->flags &= ~((uint16_t)FLAG_PREC);
                                }
                                ctxt->precision = precision;
                        }

                        // length modifiers:
                        ctxt->lmods[0] = 0;
                        ctxt->lmods[1] = 0;
                        int k = 0;
                        while(format[i] == 'l' || format[i] == 'h' || format[i] == 'z' || format[i] == 't' || format[i] == 'j' || format[i] == 'L') {
                                // absorb all length modifiers
                                if (k < 2) {
                                        // normally, no more than 2 length modifiers,
                                        // in a correct format specification, i.e. lld, hhu...
                                        // if more appear, we only count the first 2:
                                        ctxt->lmods[k++] = format[i++];
                                } else {
                                        goto bad_format;
                                }
                        }

                        // argument conversion:
                        // incorrect length modifiers, e.g. lh, hz, etc...
                        // or undefined length modifiers for the conversion specifier, e.g. Ld, zf, etc...
                        // ...will be ignored

                        ctxt->specifier = format[i++];
                        switch(ctxt->specifier) {
                                case 'i':
                                case 'd':
                                case 'u':
                                case 'X':
                                case 'x':
                                case 'o':
                                case 'p':
                                        output_int(ctxt, ap);

                                case 'E':
                                case 'F':
                                case 'G':
                                        ctxt->flags |= FLAG_UCAS;
                                        ctxt->specifier -= 'A' - 'a'; // set specifier to lower case
                                        // fall through
                                case 'e':
                                case 'f':
                                case 'g':
                                        output_fp(ctxt, ap);
                                        break;

                                case 'c':
                                case 's':
                                        output_str(ctxt, ap);
                                        break;

                                case 'n':
                                        output_n(ctxt, ap);
                                        continue; // back to main loop -> while(format[i] != '\0') {...}

                                case '%':
                                        output_char(ctxt, '%');
                                        continue; // back to main loop -> while(format[i] != '\0') {...}

                                default:
                                        bad_format:
                                        return FORMAT_ERR;
                        }

                } // end of 'if (format[i] == '%')'

        } // end of main loop

        append_nul(ctxt);
        return ctxt->total_needed;
}




// printf-family functions

int bootmode_vsnprintf(char *str, size_t size, const char *format, bm_va_list ap) {
        if (str == NULL) {
                return STROUT_NUL;
        }
        bm_output_ctxt ctxt;
        initiate_ctxt(&ctxt, str);
        set_character_limit(&ctxt, size);
        int res = main_output_loop(&ctxt, format, ap);
        return res;
}

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap) {
        if (str == NULL) {
                return STROUT_NUL;
        }
        bm_output_ctxt ctxt;
        initiate_ctxt(&ctxt, str);
        int res = main_output_loop(&ctxt, format, ap);
        return res;
}

int bootmode_vprintf(const char *format, bm_va_list ap) {
        bm_output_ctxt ctxt;
        initiate_ctxt(&ctxt, NULL);
        int res = main_output_loop(&ctxt, format, ap);
        return res;
}

int bootmode_snprintf(char *str, size_t size, const char *format, ...) {
        bm_va_list ap;
        bm_va_start(ap, format);
        int res = bootmode_vsnprintf(str, size, format, ap);
        bm_va_end(ap);
        return res;
}

int bootmode_sprintf(char *str, const char *format, ...) {
        bm_va_list ap;
        bm_va_start(ap, format);
        int res = bootmode_vsprintf(str, format, ap);
        bm_va_end(ap);
        return res;
}

int bootmode_printf(const char *format, ...) {
        bm_va_list ap;
        bm_va_start(ap, format);
        int res = bootmode_vprintf(format, ap);
        bm_va_end(ap);
        return res;
}
