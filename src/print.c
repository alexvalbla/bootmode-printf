#include "formatting.h"



void main_output_loop(bm_output_ctxt *ctxt, const char *format, bm_va_list ap) {
        size_t i = 0; // used to keep track of where we are on the format string

        // main loop
        while(format[i] != '\0') {
                if (format[i] != '%') {
                        // most common case: simply output the charater
                        output_char(ctxt, format[i++]);
                } else {
                        // this is where the fun begins...
                        REMOVE_FORMATTING_FLAGS(ctxt->flags); // clean slate

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
                        if (ctxt->flags&FLAG_LADJ) {
                                // - flags overrides 0 flag
                                ctxt->flags &= ~((uint16_t)(FLAG_ZERO));
                        }

                        // field width modifier:
                        if (format[i] >= '1' && format[i] <= '9') {
                                ctxt->flags |= FLAG_WDTH;
                                uint16_t field_width = format[i++] - '0';
                                while (format[i] >= '0' && format[i] <= '9') {
                                        field_width = field_width*10 + (format[i++]-'0');
                                        if (field_width > MAX_FIELD_WIDTH) {
                                                // safeguard
                                                field_width = MAX_FIELD_WIDTH;
                                        }
                                }
                                ctxt->field_width = field_width;
                        }

                        // precision modifier:
                        if (format[i] == '.') {
                                ctxt->flags |= FLAG_PREC;
                                uint16_t precision = 0;
                                ++i;
                                while(format[i] >= '0' && format[i] <= '9') {
                                        precision = precision*10 + (format[i++]-'0');
                                        if (precision > MAX_PREC) {
                                                precision = MAX_PREC;
                                        }
                                }
                                ctxt->precision = precision;
                        }

                        // length modifiers:
                        int k = 0;
                        while(format[i] == 'l' || format[i] == 'h' || format[i] == 'L' || format[i] == 'z') {
                                // absorb all length modifiers
                                if (k < 2) {
                                        // normally, no more than 2 length modifiers,
                                        // in a correct format specification, i.e. lld, hhu...
                                        // if more appear, we only count the first 2:
                                        ctxt->lmods[k++] = format[i];
                                }
                                ++i;
                        }

                        // argument conversion:
                        // incorrect length modifiers, e.g. lh, hz, etc...
                        // or undefined length modifiers for the conversion specifier, e.g. Ld, zf, etc...
                        // ...will be ignored

                        ctxt->specifier = format[i++];
                        switch(ctxt->specifier) {
                                case 'i':
                                case 'd':
                                        output_d(ctxt, ap);
                                        break;

                                case 'u':
                                        output_u(ctxt, ap);
                                        break;

                                case 'X':
                                        ctxt->flags |= FLAG_UCAS;
                                        // fall through
                                case 'x':
                                        output_x(ctxt, ap);
                                        break;

                                case 'o':
                                        output_o(ctxt, ap);
                                        break;

                                case 'p':
                                        output_p(ctxt, ap);
                                        break;

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
                                        output_c(ctxt, ap);
                                        break;

                                case 's':
                                        output_s(ctxt, ap);
                                        break;

                                case 'n':
                                        output_n(ctxt, ap);
                                        continue; // back to main loop -> while(format[i] != '\0') {...}

                                case '%':
                                        output_char(ctxt, '%');
                                        continue; // back to main loop -> while(format[i] != '\0') {...}

                                default:
                                        // bad specifier
                                        output_char(ctxt, ctxt->specifier);
                                        break;
                                        // goto BAD_FORMAT;
                        }

        //
        //                 if ((flags&FLAG_WDTH) && length < field_width) {
        //                         pad_conversion(format[i], tmp, flags, length, field_width);
        //                         length = field_width;
        //                 }
        //                 tmp[length] = '\0'; //should already be there, but just in case...
        //
        //                 //remember: need to reserve 1 byte for '\0' character
        //                 //total is the number of character needed so far
        //                 //BEFORE appending the new conversion
        //                 if (total+1 < size) {
        //                         size_t space_left = size-total-1;
        //                         if (total+length < size) {
        //                                 //space to copy the whole conversion
        //                                 for(size_t i = 0; i < length; i++) {
        //                                         str[str_idx++] = tmp[i];
        //                                 }
        //                         }
        //                         else{
        //                                 //copy strwhat you have space for
        //                                 for(size_t i = 0; i < space_left; i++) {
        //                                         str[str_idx++] = tmp[i];
        //                                 }
        //                         }
        //                 }
        //                 total += length;
        //                 i++; //point to next format character
        //                 continue; //back to beginning of main 'while' loop
        //
        //                 BAD_FORMAT:
        //                 if (format[i] != '\0') {
        //                         total++;
        //                         if (total < size) {
        //                                 //room for (at least) one more character
        //                                 //before terminating null byte
        //                                 str[str_idx++] = format[i];
        //                         }
        //                         i++;
        //                 }
        //                 else{
        //                         str[str_idx] = '\0';
        //                         return (int)total;
        //                 }
                } // end of 'if (format[i] == '%')'
        } // end of main 'while' loop

        // printf("%zu\n", ctxt->total_written);
        append_nul(ctxt);
        // return ctxt->total_needed;
}




// printf-family functions

int bootmode_vsnprintf(char *str, size_t size, const char *format, bm_va_list ap) {
        bm_output_ctxt ctxt;
        initiate_ctxt(&ctxt, str);
        set_character_limit(&ctxt, size);
        main_output_loop(&ctxt, format, ap);
        return ctxt.total_needed;
}

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap) {
        bm_output_ctxt ctxt;
        initiate_ctxt(&ctxt, str);
        main_output_loop(&ctxt, format, ap);
        return ctxt.total_written;
}

int bootmode_vprintf(const char *format, bm_va_list ap) {
        bm_output_ctxt ctxt;
        initiate_ctxt(&ctxt, NULL);
        main_output_loop(&ctxt, format, ap);
        return ctxt.total_written;
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
