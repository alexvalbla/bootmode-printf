#include "formatting.h"


static inline void output_to_stdout(char *str){
        //unlike puts(), does not append '\n' character
        while(*str){
                putchar(*(str++));
        }
}

int main_output_loop(bm_output_ctxt *ctxt, const char *format, bm_va_list ap) {
        size_t i = 0; // used to keep track of where we are on the format string

        //main loop
        while(format[i] != '\0') {
                if(format[i] != '%') {
                        // most common case: simply output the charater
                        output_char(ctxt, format[i++]);
                }
                else {
                        // this is where the fun begins...

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
                        if(format[i] >= '1' && format[i] <= '9') {
                                ctxt->flags |= FLAG_WDTH;
                                uint16_t field_width = format[i++] - '0';
                                while(format[i] >= '0' && format[i] <= '9') {
                                        field_width = field_width*10 + (format[i++]-'0');
                                        if(field_width > MAX_FIELD_WIDTH) {
                                                // safeguard
                                                field_width = MAX_FIELD_WIDTH;
                                        }
                                }
                                ctxt->field_width = field_width;
                        }

                        // precision modifier:
                        if(format[i] == '.'){
                                ctxt->flags |= FLAG_PREC;
                                uint16_t precision = 0;
                                ++i;
                                while(format[i] >= '0' && format[i] <= '9'){
                                        precision = precision*10 + (format[i++]-'0');
                                        if(precision > MAX_PREC){
                                                precision = MAX_PREC;
                                        }
                                }
                                ctxt->precision = precision;
                        }

                        // length modifiers:
                        int k = 0;
                        while(format[i] == 'l' || format[i] == 'h' || format[i] == 'L' || format[i] == 'z'){
                                // absorb all length modifiers
                                if(k < 2){
                                        // normally, no more than 2 length modifiers,
                                        // in a correct format specification, i.e. lld, hhu...
                                        // if more appear, we only count the first 2:
                                        ctxt->lmods[k++] = format[i];
                                }
                                i++;
                        }

                        // argument conversion:
                        // incorrect length modifiers, e.g. lh, hz, etc...
                        // or undefined length modifiers for the conversion specifier, e.g. Ld, zf, etc...
                        // ...will be ignored

                        switch(format[i]){
                                case 'i':
                                case 'd':
                                        output_d(ctxt, ap);
                                        break;
                                //
                                // case 'u':
                                //         length = output_u(ap, mods, tmp, prec, flags);
                                //         break;
                                //
                                // case 'X':
                                //         flags |= FLAG_UCAS;
                                //         // fall through
                                // case 'x':
                                //         length = output_x(ap, mods, tmp, prec, flags);
                                //         break;
                                //
                                // case 'o':
                                //         length = output_o(ap, mods, tmp, prec, flags);
                                //         break;
                                //
                                // case 'p':
                                //         length = output_p(ap, tmp, prec, flags);
                                //         break;
                                //
                                // case 'E':
                                //         flags |= FLAG_UCAS;
                                //         // fall thstrrough
                                // case 'e':
                                //         length = output_e(ap, mods, tmp, prec, flags);
                                //         break;
                                //
                                // case 'F':
                                //         flags |= FLAG_UCAS;
                                //         // fall through
                                // case 'f':
                                //         length = output_f(ap, mods, tmp, prec, flags);
                                //         break;
                                //
                                // case 'G':
                                //         flags |= FLAG_UCAS;
                                //         // fall through
                                // case 'g':
                                //         length = output_g(ap, mods, tmp, prec, flags);
                                //         break;
                                //
                                // case 'c':
                                //         length = output_c(ap, tmp);
                                //         break;
                                //
                                // case 's':
                                //         length = output_s(ap, tmp, prec, flags);
                                //         break;
                                //
                                // case 'n':
                                //         output_n(ap, mods, total);
                                //         i++;
                                //         continue; // back to main loop -> while(format[i] != '\0') {...}
                                //
                                // case '%':
                                //         total++;
                                //         if(total < size){
                                //                 str[str_idx++] = '%';
                                //         }
                                //         i++;
                                //         continue; // back to main loop -> while(format[i] != '\0') {...}

                                default:
                                        return 1;
                                        // goto BAD_FORMAT;
                        }

        //
        //                 if((flags&FLAG_WDTH) && length < field_width){
        //                         pad_conversion(format[i], tmp, flags, length, field_width);
        //                         length = field_width;
        //                 }
        //                 tmp[length] = '\0'; //should already be there, but just in case...
        //
        //                 //remember: need to reserve 1 byte for '\0' character
        //                 //total is the number of character needed so far
        //                 //BEFORE appending the new conversion
        //                 if(total+1 < size){
        //                         size_t space_left = size-total-1;
        //                         if(total+length < size){
        //                                 //space to copy the whole conversion
        //                                 for(size_t i = 0; i < length; i++){
        //                                         str[str_idx++] = tmp[i];
        //                                 }
        //                         }
        //                         else{
        //                                 //copy strwhat you have space for
        //                                 for(size_t i = 0; i < space_left; i++){
        //                                         str[str_idx++] = tmp[i];
        //                                 }
        //                         }
        //                 }
        //                 total += length;
        //                 i++; //point to next format character
        //                 continue; //back to beginning of main 'while' loop
        //
        //                 BAD_FORMAT:
        //                 if(format[i] != '\0'){
        //                         total++;
        //                         if(total < size){
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
        //         } //end of 'if(format[i] == '%')'
        //
                }
        } //end of main 'while' loop

        // str[str_idx] = '\0';
        // return (int)total;
        return ctxt->total_needed;

        //NOTES:
        //writes at most size-1 bytes to str
        //always appends a '\0' charater
        //if total >= size, then not verything was written
        //returns number of characters necessary...
        //...if str had had sufficient memory
}

int bootmode_vsnprintf(char *str, size_t size, const char *format, bm_va_list ap) {
        return 1;
}



//derivatives

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap){
        int res;
        res = bootmode_vsnprintf(str, SIZE_MAX, format, ap);
        return res;
}

int bootmode_vprintf(const char *format, bm_va_list ap){
        char buff[DFLT_SIZE];
        int res;
        bm_va_list aq;
        bm_va_copy(aq,ap);
        res = bootmode_vsnprintf(buff, DFLT_SIZE, format, ap);
        if(res >= DFLT_SIZE){
                char *str = (char *)alloca((res+1)*sizeof(char));
                bootmode_vsnprintf(str, res+1, format, aq);
                output_to_stdout(str);
        }
        else{
                output_to_stdout(buff);
        }
        bm_va_end(aq);
        return res;
}

int bootmode_snprintf(char *str, size_t size, const char *format, ...){
        bm_va_list ap;
        bm_va_start(ap, format);
        int res = bootmode_vsnprintf(str, size, format, ap);
        bm_va_end(ap);
        return res;
}

int bootmode_sprintf(char *str, const char *format, ...){
        bm_va_list ap;
        bm_va_start(ap, format);
        int res = bootmode_vsprintf(str, format, ap);
        bm_va_end(ap);
        return res;
}

int bootmode_printf(const char *format, ...){
        bm_va_list ap;
        bm_va_start(ap, format);
        int res = bootmode_vprintf(format, ap);
        bm_va_end(ap);
        return res;
}
