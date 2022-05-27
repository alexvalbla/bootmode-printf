#include "formatting.h"
#include "conversion.h"
#include "output_utils.h"



static void str_rev(char *s, size_t size) {
        char *p = s + size;
        --p;
        while (s < p) {
                *s ^= *p;
                *p ^= *s;
                *s ^= *p;
                ++s;
                --p;
        }
}

static unsigned long long extract_integer(bm_va_list ap, char *lmods) {
        unsigned long long a;
        if (lmods[0] == 'l' && lmods[1] == '\0') {
                a = bm_va_arg(ap, long);
        }
        else if (lmods[0] == 'l' && lmods[1] == 'l') {
                a = bm_va_arg(ap, long long);
        }
        else if (lmods[0] == 'h' && lmods[1] == '\0') {
                a = bm_va_arg(ap, int);
        }
        else if (lmods[0] == 'h' && lmods[1] == 'h') {
                a = bm_va_arg(ap, int);
        }
        else if (lmods[0] == 'z' && lmods[1] == '\0') {
                a = bm_va_arg(ap, size_t);
        }
        else {
                a = bm_va_arg(ap, int);
        }
        return a;
}

int int_fmt_d(char *conv_buff, uint64_t a) {
        int i = 0;
        while (a != 0) {
                conv_buff[i++] = '0' + a%10;
                a /= 10;
        }
        str_rev(conv_buff, i);
        conv_buff[i] = '\0';
        return i;
}

int int_fmt_x(char *conv_buff, uint64_t a, uint16_t flags) {
        int i = 0;
        while (a != 0) {
                char hex = a & 0x0F; // 0x0F ==  b'1111' -> get 4 bits of lowest order
                if (hex < 10) {
                        conv_buff[i++] = '0' + hex;
                } else {
                        hex = hex - 10 + 'a';
                        if (flags&FLAG_UCAS) {
                                hex += 'A' - 'a'; // switch to upper case
                        }
                        conv_buff[i++] = hex;
                }
                a >>= 4;
        }
        str_rev(conv_buff, i);
        conv_buff[i] = '\0';
        return i;
}

int int_fmt_o(char *conv_buff, uint64_t a) {
        int i = 0;
        while (a != 0) {
                char oct = a & 7; // 7 ==  b'111' -> get 3 bits of lowest order
                conv_buff[i++] = '0' + oct;
                a >>= 3;
        }
        str_rev(conv_buff, i);
        conv_buff[i] = '\0';
        return i;
}

void output_d(bm_output_ctxt *ctxt, bm_va_list ap) {
        int64_t a = (int64_t)extract_integer(ap, ctxt->lmods);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        char *conv_buff = &(ctxt->conv_buff[0]);

        uint64_t b;
        if (a < 0) {
                b = -a;
        }
        else {
                b = a;
        }
        int digits = int_fmt_d(conv_buff, b); // reminder: if a == 0, digits == 0

        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }
        if (digits == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                digits = 1;
        }

        int total_length = digits + 1; // provisional +1 for sign character
        if (a < 0) {
                output_char(ctxt, '-');
        } else if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        } else {
                --total_length; // remove provisional +1
        }

        int prec_padding = precision - digits;
        if (prec_padding > 0) {
                output_char_loop(ctxt, '0', prec_padding);
        }
        output_buffer(ctxt, conv_buff, digits);
}

void output_u(bm_output_ctxt *ctxt, bm_va_list ap) {
        uint64_t a = (uint64_t)extract_integer(ap, ctxt->lmods);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        char *conv_buff = &(ctxt->conv_buff[0]);

        int digits = int_fmt_d(conv_buff, a); // reminder: if a == 0, digits == 0

        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }
        if (digits == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                digits = 1;
        }

        int total_length = digits + 1; // provisional +1 for sign character
        if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        } else {
                --total_length; // remove provisional +1
        }

        int prec_padding = precision - digits;
        if (prec_padding > 0) {
                output_char_loop(ctxt, '0', prec_padding);
        }
        output_buffer(ctxt, conv_buff, digits);
}

void output_x_inner(bm_output_ctxt *ctxt, uint64_t a);

void output_x(bm_output_ctxt *ctxt, bm_va_list ap) {
        uint64_t a = (uint64_t)extract_integer(ap, ctxt->lmods);
        output_x_inner(ctxt, a);
}

void output_x_inner(bm_output_ctxt *ctxt, uint64_t a) {
        // sub-function that can be used for %p as well
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }

        char *conv_buff = &(ctxt->conv_buff[0]);
        int buff_len = int_fmt_x(conv_buff, a, flags);
        if (buff_len == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                buff_len = 1;
        }

        if ((flags&FLAG_ALTF) && a > 0) {
                output_char(ctxt, '0');
                char x = 'x';
                if (flags&FLAG_UCAS) {
                        x = 'X';
                }
                output_char(ctxt, x);
        }

        int prec_padding = precision - buff_len;
        if (prec_padding > 0) {
                output_char_loop(ctxt, '0', prec_padding);
        }
        output_buffer(ctxt, conv_buff, buff_len);
}

void output_o(bm_output_ctxt *ctxt, bm_va_list ap) {
        uint64_t a = (uint64_t)extract_integer(ap, ctxt->lmods);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }

        char *conv_buff = &(ctxt->conv_buff[0]);
        int buff_len = int_fmt_o(conv_buff, a);
        if (buff_len == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                buff_len = 1;
        }

        int digits = buff_len;
        if (flags&FLAG_ALTF && conv_buff[0] != '0') {
                output_char(ctxt, '0');
                ++digits;
        }

        int prec_padding = precision - digits;
        if (prec_padding > 0) {
                output_char_loop(ctxt, '0', prec_padding);
        }
        output_buffer(ctxt, conv_buff, buff_len);
}

void output_p(bm_output_ctxt *ctxt, bm_va_list ap) {
        // equivalent to %#x or %#lx
        ctxt->flags |= FLAG_ALTF;
        void *p = bm_va_arg(ap, void *);
        output_x_inner(ctxt, (uint64_t)p);
}

void output_c(bm_output_ctxt *ctxt, bm_va_list ap) {
        char c = (char)bm_va_arg(ap, int);
        output_char(ctxt, c);
}

void output_s(bm_output_ctxt *ctxt, bm_va_list ap) {
        const char *s = bm_va_arg(ap, const char *);
        size_t len;
        if (s == NULL) {
                s = "(null)";
                len = 6;
        } else {
                const char *cursor = s;
                while (*(cursor++)) {}
                len = cursor-s;
        }
        output_buffer(ctxt, s, len);
}

void output_n(bm_output_ctxt *ctxt, bm_va_list ap) {
        char *lmods = &(ctxt->lmods[0]);
        if (lmods[0] == 'l' && lmods[1] == '\0') {
                long *n = bm_va_arg(ap, long*);
                *n = ctxt->total_written;
        }
        else if (lmods[0] == 'l' && lmods[1] == 'l') {
                long long *n = bm_va_arg(ap, long long*);
                *n = ctxt->total_written;
        }
        else if (lmods[0] == 'h' && lmods[1] == '\0') {
                short *n = (short*)bm_va_arg(ap, int*);
                *n = ctxt->total_written;
        }
        else if (lmods[0] == 'h' && lmods[1] == 'h') {
                char *n = (char*)bm_va_arg(ap, int*);
                *n = ctxt->total_written;
        }
        else if (lmods[0] == 'z' && lmods[1] == '\0') {
                size_t *n = bm_va_arg(ap, size_t*);
                *n = ctxt->total_written;
        }
        else {
                int *n = bm_va_arg(ap, int*);
                *n = ctxt->total_written;
        }
}

// int output_e(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags) {
//         char s; //sign
//         int32_t E; //binary exponent
//         uint64_t m; //binary mantissa
//         int32_t F; //decimal exponent
//         uint64_t n; //decimal mantissa
//         fpclass_t class;
//         if (mods[0] == 'L' && mods[1] == '\0') {
//                 class = decomposeLongDouble(&s, &E, &m, bm_va_arg(ap, long double));
//         }
//         else {
//                 class = decomposeDouble(&s, &E, &m, bm_va_arg(ap, double));
//         }
//         if (class == BM_NUMBER) {
//                 if (m == 0) {
//                         n = 0;
//                 }
//                 decimalConversion(&F, &n, E, m);
//                 return fp_fmt_e(str, s, n, F, prec, flags);
//         }
//         //otherwise: +inf, -inf or nan
//         return fp_special_case(class, str, flags);
// }
//
// int output_f(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags) {
//         char s; //sign
//         int32_t E; //binary exponent
//         uint64_t m; //binary mantissa
//         int32_t F; //decimal exponent
//         uint64_t n; //decimal mantissa
//         fpclass_t class;
//         if (mods[0] == 'L' && mods[1] == '\0') {
//                 class = decomposeLongDouble(&s, &E, &m, bm_va_arg(ap, long double));
//         }
//         else {
//                 class = decomposeDouble(&s, &E, &m, bm_va_arg(ap, double));
//         }
//         if (class == BM_NUMBER) {
//                 if (m == 0) {
//                         n = 0;
//                 }
//                 decimalConversion(&F, &n, E, m);
//                 return fp_fmt_f(str, s, n, F, prec, flags);
//         }
//         //otherwise: +inf, -inf or nan
//         return fp_special_case(class, str, flags);
// }
//
// int output_g(bm_va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags) {
//         char s; //sign
//         int32_t E; //binary exponent
//         uint64_t m; //binary mantissa
//         int32_t F; //decimal exponent
//         uint64_t n; //decimal mantissa
//         fpclass_t class;
//         if (mods[0] == 'L' && mods[1] == '\0') {
//                 class = decomposeLongDouble(&s, &E, &m, bm_va_arg(ap, long double));
//         }
//         else {
//                 class = decomposeDouble(&s, &E, &m, bm_va_arg(ap, double));
//         }
//         if (class == BM_NUMBER) {
//                 if (m == 0) {
//                         n = 0;
//                 }
//                 decimalConversion(&F, &n, E, m);
//                 return fp_fmt_g(str, s, n, F, prec, flags);
//         }
//         //otherwise: +inf, -inf or nan
//         return fp_special_case(class, str, flags);
// }



//floating point formatting functions
//
// int fp_fmt_e(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags) {
//         //adjust precision
//         if (flags&FLAG_PREC) {
//                 if (prec > 18) {
//                         prec = 18;
//                 }
//         }
//         else {
//                 //no precision specified, taken as 6
//                 prec = 6;
//         }
//
//         //write sign
//         int i = 0;
//         if (s) {
//                 str[i++] = '-';
//         }
//         else if (flags&FLAG_SIGN) {
//                 str[i++] = '+';
//         }
//         else if (flags&FLAG_WSPC) {
//                 str[i++] = ' ';
//         }
//
//         if (n == 0) {
//                 //f == 0.
//                 str[i++] = '0';
//                 if (prec || (flags&FLAG_ALTF)) {
//                         str[i++] = '.';
//                         for(int j = 0; j < prec; j++) {
//                                 str[i++] = '0';
//                         }
//                 }
//                 str[i++] = 'e';
//                 str[i++] = '+';
//                 str[i++] = '0';
//                 str[i++] = '0';
//                 str[i] = '\0';
//                 return i;
//         }
//
//         int l = 20;
//         char d[l]; //to write decimal mantissa
//         char e[l]; //to write decimal exponent
//         //n has 19 digits
//         int_fmt_u(n,d,0,0);
//         //we want 1 before the decimal point, 18 after
//         //so we increase the decimal exponent by 18
//         F += 18;
//         if (F > -10 && F < 10) {
//                 //we want e to be written with at least 2 digits
//                 //example: e-05, e+02, e+00
//                 int k = 0;
//                 if (F < 0) {
//                         e[k++] = '-';
//                         F = -F;
//                 }
//                 e[k++] = '0';
//                 e[k++] = '0'+F;
//                 e[k] = '\0';
//         }
//         else {
//                 int_fmt_d(F,e,0,0);
//         }
//
//         //write decimal mantissa
//         str[i++] = d[0];
//         if (prec || (flags&FLAG_ALTF)) {
//                 str[i++] = '.';
//                 for(int j = 1; j <= prec; j++) {
//                         str[i++] = d[j];
//                 }
//         }
//
//         //write decimal exponent
//         str[i++] = 'e';
//         int j = 0;
//         if (e[j] == '-') {
//                 str[i++] = e[j++];
//         }
//         else {
//                 str[i++] = '+';
//         }
//         while (e[j]) {
//                 str[i++] = e[j++];
//         }
//         str[i] = '\0';
//         return i;
// }
//
// int fp_fmt_f(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags) {
//         //adjust precision
//         if (flags&FLAG_PREC) {
//                 if (prec > 19) {
//                         prec = 19;
//                 }
//         }
//         else {
//                 //no precision specified, taken as 6
//                 prec = 6;
//         }
//
//         //write sign
//         int i = 0;
//         if (s) {
//                 str[i++] = '-';
//         }
//         else if (flags&FLAG_SIGN) {
//                 str[i++] = '+';
//         }
//         else if (flags&FLAG_WSPC) {
//                 str[i++] = ' ';
//         }
//
//         if (n == 0) {
//                 //f == 0.
//                 str[i++] = '0';
//                 if (prec) {
//                         str[i++] = '.';
//                         for(int j = 0; j < prec; j++) {
//                                 str[i++] = '0';
//                         }
//                 }
//                 str[i] = '\0';
//                 return i;
//         }
//
//         int l = 20;
//         char d[l]; //to write decimal mantissa
//         //n has 19 digits
//         int_fmt_u(n,d,0,0);
//
//         //write decimal mantissa
//         int digits = 19+F; //digits before decimal point
//         int j = 0; //used to index decimal mantissa
//         if (digits >= 19) {
//                 //write the whole mantissa
//                 //then add zeros until decimal point
//                 //add prec zeros after decimal point
//                 for(j = 0; j < 19; j++) {
//                         str[i++] = d[j];
//                 }
//                 while (j < digits) {
//                         str[i++] = '0';
//                         j++;
//                 }
//                 if (prec || (flags&FLAG_ALTF)) {
//                         str[i++] = '.';
//                         for(int k = 0; k < prec; k++) {
//                                 str[i++] = '0';
//                         }
//                 }
//         }
//         else if (digits > 0) {
//                 //write part of the decimal mantissa before decimal point
//                 //then maybe write the rest afterwards, depending on prec
//                 //complete with zeros if not enough digits for prec
//                 for(j = 0; j < digits; j++) {
//                         str[i++] = d[j];
//                 }
//                 if (prec || (flags&FLAG_ALTF)) {
//                         str[i++] = '.';
//                         int decimals = 19-j; //digits on decimal mantissa...
//                         //...that we haven't consumed yet
//                         for(int k = 0; k < prec; k++) {
//                                 if (k < decimals) {
//                                         str[i++] = d[j++];
//                                 }
//                                 else {
//                                         str[i++] = '0';
//                                 }
//                         }
//                 }
//         }
//         else {
//                 //0.0...0dddd
//                 //the decimal mantissa starts
//                 //after the decimal point
//                 str[i++] = '0';
//                 if (prec || (flags&FLAG_ALTF)) {
//                         str[i++] = '.';
//                         digits = -digits;
//                         for(int k = 0, j = 0; k < prec; k++) {
//                                 if (k < digits) {
//                                         str[i++] = '0';
//                                 }
//                                 else {
//                                         str[i++] = d[j++];
//                                 }
//                         }
//                 }
//         }
//         str[i] = '\0';
//         return i;
// }
//
// int fp_fmt_g(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags) {
//         //adjust precision
//         if (flags&FLAG_PREC) {
//                 if (prec > 19) {
//                         prec = 19;
//                 }
//                 else if (prec == 0) {
//                         prec = 1;
//                 }
//         }
//         else {
//                 //no precision specified, taken as 6
//                 prec = 6;
//         }
//
//         //write sign
//         int i = 0;
//         if (s) {
//                 str[i++] = '-';
//         }
//         else if (flags&FLAG_SIGN) {
//                 str[i++] = '+';
//         }
//         else if (flags&FLAG_WSPC) {
//                 str[i++] = ' ';
//         }
//
//         if (n == 0) {
//                 //f == 0.
//                 str[i++] = '0';
//                 if (flags&FLAG_ALTF) {
//                         str[i++] = '.';
//                         for(int k = 1; k < prec; k++) {
//                                 str[i++] = '0';
//                         }
//                 }
//                 str[i] = '\0';
//                 return i;
//         }
//         int l = 20;
//         char d[l]; //to write decimal mantissa
//         char e[l]; //to write decimal exponent
//         //n has 19 digits
//         int_fmt_u(n,d,0,0);
//
//         //if written in 'e'-format, the exponent would be F+18
//         if (F+18 >= (int)prec || F+18 < -4) {
//                 //'e'-format conversion
//                 F += 18;
//                 int_fmt_d(F,e,0,0);
//                 str[i++] = d[0];
//                 if (prec > 1 || (flags&FLAG_ALTF)) {
//                         str[i++] = '.';
//                         for(int j = 1; j < prec; j++) {
//                                 str[i++] = d[j];
//                         }
//                         if (!(flags&FLAG_ALTF)) {
//                                 //remove trailing zeros
//                                 //and decimal point if no decimals follow it
//                                 while (str[i-1] == '0') {
//                                         i--;
//                                 }
//                                 if (str[i-1] == '.') {
//                                         i--;
//                                 }
//                         }
//                 }
//                 int k = 0;
//                 str[i++] = 'e';
//                 if (e[k] == '-') {
//                         str[i++] = e[k++];
//                 }
//                 else {
//                         str[i++] = '+';
//                 }
//                 if (e[k] == '0') {
//                         //exponent is zero, has to be written as 00
//                         str[i++] = '0';
//                         str[i++] = '0';
//                 }
//                 else {
//                         if (F > -10 && F < 10) {
//                                 str[i++] = '0';
//                         }
//                         while (e[k]) {
//                                 str[i++] = e[k++];
//                         }
//                 }
//                 str[i] = '\0';
//                 return i;
//         }
//
//         //'f'-format conversion
//         int digits = 19+F; //digits before the decimal point
//         int j = 0; //where we are on our decimal mantissa
//         if (digits <= 0) {
//                 //0.0... ddd
//                 str[i++] = '0';
//         }
//         else {
//                 for(int k = 0; k < digits; k++) {
//                         str[i++] = d[j++];
//                 }
//         }
//         if (j < prec || (flags&FLAG_ALTF)) {
//                 str[i++] = '.';
//                 while (digits < 0) {
//                         str[i++] = '0';
//                         digits++;
//                 }
//                 while (j < prec) {
//                         str[i++] = d[j++];
//                 }
//                 //remove trailing zeros
//                 if (!(flags&FLAG_ALTF)) {
//                         //remove trailing zeros
//                         //and decimal point if no decimals follow it
//                         while (str[i-1] == '0') {
//                                 i--;
//                         }
//                         if (str[i-1] == '.') {
//                                 i--;
//                         }
//                 }
//         }
//         str[i] = '\0';
//         return i;
// }
//
// int fp_special_case(fpclass_t class, char *str, uint8_t flags) {
//         int i = 0;
//         if (class == BM_NAN) {
//                 str[i++] = 'n';
//                 str[i++] = 'a';
//                 str[i++] = 'n';
//                 str[i] = '\0';
//         }
//         else if (class == BM_POS_INF) {
//                 if (flags&FLAG_SIGN) {
//                         str[i++] = '+';
//                 }
//                 else if (flags&FLAG_WSPC) {
//                         str[i++] = ' ';
//                 }
//                 str[i++] = 'i';
//                 str[i++] = 'n';
//                 str[i++] = 'f';
//                 str[i] = '\0';
//         }
//         else {
//                 //BM_NEG_INF
//                 str[i++] = '-';
//                 str[i++] = 'i';
//                 str[i++] = 'n';
//                 str[i++] = 'f';
//                 str[i] = '\0';
//         }
//         return i;
// }
//
//
//
//
// //for padding after conversions
//
// void pad_conversion(char fmt, char *str, uint8_t flags, uint16_t length, uint16_t field_width) {
//         //str contains the argument conversion
//         //supposes that field_width > length
//         int l = field_width-length;
//         int integer = 0;
//         char c = ' ';
//         if (!(flags&FLAG_LADJ) && flags&FLAG_ZERO) {
//                 c = '0';
//         }
//         switch (fmt) {
//                 //integer conversions
//                 case 'i':
//                 case 'd':
//                 case 'u':
//                 case 'X':
//                 case 'x':
//                 case 'o':
//                 case 'p':
//                 if (flags&FLAG_PREC) {
//                         //precision was given, ignore 0 flag
//                         c = ' ';
//                 }
//                 integer = 1;
//                 // fall through
//
//                 case 'E':
//                 case 'e':
//                 case 'F':
//                 case 'f':
//                 case 'G':
//                 case 'g':
//                 if (flags&FLAG_LADJ) {
//                         //pad with whitespace on the right (left adjusted)
//                         pad_str(&str[length], l, c);
//                 }
//                 else {
//                         if (c == '0' && integer) {
//                                 //pad with zeros on the left (right adjusted)
//                                 if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
//                                         //0x... or 0X...
//                                         //padding will be 0x0000000...
//                                         //rather than     00000000x...
//                                         shift_str(&str[2], l, length-2);
//                                         pad_str(&str[2], l, '0');
//                                 }
//                                 else if (str[0] == ' ' || str[0] == '-' || str[0] == '+') {
//                                         //sign comes before the zeros
//                                         shift_str(&str[1], l, length-1);
//                                         pad_str(&str[1], l, '0');
//                                 }
//                                 else {
//                                         shift_str(str, l, length);
//                                         pad_str(str, l, '0');
//                                 }
//                         }
//                         else if (c == '0' && !integer) {
//                                 //floating point padded with zeros
//                                 if (str[0] == 'n' || str[0] == 'i') {
//                                         //"nan" of "inf"
//                                         //pad with whitespace, not zeros
//                                         shift_str(str, l, length);
//                                         pad_str(str, l, ' ');
//                                 }
//                                 else if (str[0] == ' ' || str[0] == '-' || str[0] == '+') {
//                                         if (str[1] == 'i') {
//                                                 //" inf" "-inf" or "+inf"
//                                                 //pad with whitespace, not zeros
//                                                 //and pad on the left
//                                                 shift_str(&str[0], l, length);
//                                                 pad_str(&str[0], l, ' ');
//                                         }
//                                         else {
//                                                 shift_str(&str[1], l, length);
//                                                 pad_str(&str[1], l, '0');
//                                         }
//                                 }
//                                 else {
//                                         shift_str(str, l, length);
//                                         pad_str(str, l, '0');
//                                 }
//                         }
//                         else {
//                                 //integer or floating point, c == ' '
//                                 //pad with whitespace on the left (right adjusted)
//                                 shift_str(str, l, length);
//                                 pad_str(str, l, ' ');
//                         }
//                 }
//                 break;
//
//                 case 'c':
//                 case 's':
//                 if (flags&FLAG_LADJ) {
//                         pad_str(&str[length], l, c);
//                 }
//                 else {
//                         shift_str(str, l, length);
//                         pad_str(str, l, c);
//                 }
//                 break;
//
//                 default:
//                 break;
//         }
//         str[field_width] = '\0';
// }
