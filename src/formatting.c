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

static unsigned long long extract_unsigned_integer(bm_va_list ap, char *lmods) {
        unsigned long long a;
        if (lmods[0] == 'l' && lmods[1] == '\0') {
                a = bm_va_arg(ap, unsigned long);
        }
        else if (lmods[0] == 'l' && lmods[1] == 'l') {
                a = bm_va_arg(ap, unsigned long long);
        }
        else if (lmods[0] == 'h' && lmods[1] == '\0') {
                a = bm_va_arg(ap, unsigned int);
        }
        else if (lmods[0] == 'h' && lmods[1] == 'h') {
                a = bm_va_arg(ap, unsigned int);
        }
        else if (lmods[0] == 'z' && lmods[1] == '\0') {
                a = bm_va_arg(ap, size_t);
        }
        else {
                a = bm_va_arg(ap, unsigned int);
        }
        return a;
}

static long long extract_integer(bm_va_list ap, char *lmods) {
        long long a;
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
                a = bm_va_arg(ap, ssize_t);
        }
        else {
                a = bm_va_arg(ap, int);
        }
        return a;
}

static int int_fmt_d(char *conv_buff, uint64_t a) {
        int i = 0;
        while (a != 0) {
                conv_buff[i++] = '0' + a%10;
                a /= 10;
        }
        str_rev(conv_buff, i);
        conv_buff[i] = '\0';
        return i;
}

static int int_fmt_x(char *conv_buff, uint64_t a, uint16_t flags) {
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

static int int_fmt_o(char *conv_buff, uint64_t a) {
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
        int64_t a = extract_integer(ap, ctxt->lmods);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;

        uint64_t b;
        if (a < 0) {
                b = -a;
        }
        else {
                b = a;

        }
        char conv_buff[32];
        int nb_digits = int_fmt_d(conv_buff, b); // reminder: if a == 0, nb_digits == 0

        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }
        if (nb_digits == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                nb_digits = 1;
        }

        int total_length = nb_digits + 1; // provisional +1 for sign character
        if (a < 0) {
                output_char(ctxt, '-');
        } else if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        } else {
                --total_length; // remove provisional +1
        }

        int prec_padding = precision - nb_digits;
        if (prec_padding > 0) {
                output_char_loop(ctxt, '0', prec_padding);
        }
        output_buffer(ctxt, conv_buff, nb_digits);
}

void output_u(bm_output_ctxt *ctxt, bm_va_list ap) {
        uint64_t a = extract_unsigned_integer(ap, ctxt->lmods);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;

        char conv_buff[32];
        int nb_digits = int_fmt_d(conv_buff, a); // reminder: if a == 0, nb_digits == 0

        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }
        if (nb_digits == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                nb_digits = 1;
        }

        int total_length = nb_digits + 1; // provisional +1 for sign character
        if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        } else {
                --total_length; // remove provisional +1
        }

        int prec_padding = precision - nb_digits;
        if (prec_padding > 0) {
                output_char_loop(ctxt, '0', prec_padding);
        }
        output_buffer(ctxt, conv_buff, nb_digits);
}

void output_x_inner(bm_output_ctxt *ctxt, uint64_t a);

void output_x(bm_output_ctxt *ctxt, bm_va_list ap) {
        uint64_t a = extract_unsigned_integer(ap, ctxt->lmods);
        output_x_inner(ctxt, a);
}

void output_x_inner(bm_output_ctxt *ctxt, uint64_t a) {
        // sub-function that can be used for %p as well
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }

        char conv_buff[32];
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
        uint64_t a = extract_unsigned_integer(ap, ctxt->lmods);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        if (!(flags&FLAG_PREC)) {
                precision = 1;
        }

        char conv_buff[32];
        int buff_len = int_fmt_o(conv_buff, a);
        if (buff_len == 0 && precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                conv_buff[1] = '\0';
                buff_len = 1;
        }

        int nb_digits = buff_len;
        if (flags&FLAG_ALTF && conv_buff[0] != '0') {
                output_char(ctxt, '0');
                ++nb_digits;
        }

        int prec_padding = precision - nb_digits;
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


//floating point formatting functions

static void fp_fmt_e(bm_output_ctxt *ctxt, char s, uint64_t n, int32_t F);
static void fp_fmt_f(bm_output_ctxt *ctxt, char s, uint64_t n, int32_t F);
static void fp_fmt_g(bm_output_ctxt *ctxt, char s, uint64_t n, int32_t F);
void fp_special_case(bm_output_ctxt *ctxt, fpclass_t class);


void output_fp(bm_output_ctxt *ctxt, bm_va_list ap) {
        char s; //sign
        int32_t E; //binary exponent
        uint64_t m; //binary mantissa
        int32_t F; //decimal exponent
        uint64_t n; //decimal mantissa
        fpclass_t class;
        char *lmods = &(ctxt->lmods[0]);
        if (lmods[0] == 'L' && lmods[1] == '\0') {
                class = decomposeLongDouble(&s, &E, &m, bm_va_arg(ap, long double));
        } else {
                class = decomposeDouble(&s, &E, &m, bm_va_arg(ap, double));
        }
        if (class == BM_NUMBER) {
                decimalConversion(&F, &n, E, m);
                switch (ctxt->specifier) {
                        case 'e':
                                fp_fmt_e(ctxt, s, n, F);
                                break;
                        case 'f':
                                fp_fmt_f(ctxt, s, n, F);
                                break;
                        case 'g':
                                fp_fmt_g(ctxt, s, n, F);
                                break;
                        default:
                                break;
                }
        } else {
                // otherwise: +inf, -inf or nan
                fp_special_case(ctxt, class);
        }
}

void fp_fmt_e(bm_output_ctxt *ctxt, char s, uint64_t n, int32_t F) {
        uint16_t precision = ctxt->precision;
        uint16_t flags = ctxt->flags;
        // adjust precision
        if (flags&FLAG_PREC) {
                if (precision > 18) {
                        precision = 18;
                }
        } else {
                // no precision specified, taken as 6
                precision = 6;
        }

        // sign
        if (s) {
                output_char(ctxt, '-');
        } else if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        }

        char n_array[20];
        if (n == 0) {
                // floating point == 0.
                for (int i = 0; i < 19; ++i) {
                        n_array[i] = '0';
                }
        } else {
                // write decimal mantissa
                // n has 19 digits (10^18 <= n < 10^19)
                // we want 1 digit before the decimal point
                // so we increase the decimal exponent by 18
                int_fmt_d(n_array, n);
                F += 18;
        }
        output_char(ctxt, n_array[0]);
        if (precision || (flags&FLAG_ALTF)) {
                output_char(ctxt, '.');
                output_buffer(ctxt, n_array+1, precision);
        }

        int32_t F_abs_val = (F < 0) ? -F : F;
        char F_array[10];
        int F_digits = int_fmt_d(F_array, F_abs_val);

        // write decimal exponent
        char e_adjust = (flags|FLAG_UCAS) ? 'A' - 'a' : 0; // conditionally set to upper case
        output_char(ctxt, 'e' + e_adjust);
        if (F < 0) {
                output_char(ctxt, '-');
        } else {
                output_char(ctxt, '+');
        }
        if (F_digits < 2) {
                // write with at least 2 digits
                output_char(ctxt, '0');
                if (F_digits == 1) {
                        output_char(ctxt, F_array[0]);
                } else {
                        // in other words, F == 0
                        output_char(ctxt, '0');
                }
        } else {
                output_buffer(ctxt, F_array, F_digits);
        }
}

void fp_fmt_f(bm_output_ctxt *ctxt, char s, uint64_t n, int32_t F) {
        uint16_t precision = ctxt->precision;
        uint16_t flags = ctxt->flags;
        // adjust precision
        if (flags&FLAG_PREC) {
                if (precision > 19) {
                        precision = 19;
                }
        } else {
                // no precision specified, taken as 6
                precision = 6;
        }

        // sign
        if (s) {
                output_char(ctxt, '-');
        } else if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        }

        int digits_before_point; // decimal point
        char n_array[20];
        if (n == 0) {
                // floating point == 0.
                output_char(ctxt, '0');
                if (precision > 0) {
                        output_char(ctxt, '.');
                        output_char_loop(ctxt, '0', precision);
                } else if (flags&(FLAG_ALTF)) {
                        output_char(ctxt, '.');
                }
                return;
        } else {
                digits_before_point = F+19; // n has 19 decimals
                int_fmt_d(n_array, n);
        }

        if (digits_before_point >= 19) {
                output_buffer(ctxt, n_array, 19);
                output_char_loop(ctxt, '0', digits_before_point-19);
                if (precision > 0 || flags&FLAG_ALTF) {
                        output_char(ctxt, '.');
                        output_char_loop(ctxt, '0', precision);
                }
        } else if (digits_before_point > 0) {
                output_buffer(ctxt, n_array, digits_before_point);
                if (precision > 0 || flags&FLAG_ALTF) {
                        output_char(ctxt, '.');
                        int digits_left = 19-digits_before_point;
                        if (digits_left >= precision) {
                                output_buffer(ctxt, n_array+digits_before_point, precision);
                        } else {
                                output_buffer(ctxt, n_array+digits_before_point, digits_left);
                                output_char_loop(ctxt, '0', precision-digits_left);
                        }
                }
        } else {
                output_char(ctxt, '0');
                if (precision > 0) {
                        output_char(ctxt, '.');
                        // remember that digits_before_point <= 0
                        int zeros_after_point = -digits_before_point;
                        if (zeros_after_point >= precision) {
                                output_char_loop(ctxt, '0', precision);
                        } else {
                                output_char_loop(ctxt, '0', zeros_after_point);
                                output_buffer(ctxt, n_array, precision-zeros_after_point);
                        }
                } else if (flags&FLAG_ALTF) {
                        output_char(ctxt, '.');
                }
        }
}

void fp_fmt_g(bm_output_ctxt *ctxt, char s, uint64_t n, int32_t F) {
        int32_t sig_digits = ctxt->precision;
        uint16_t flags = ctxt->flags;
        if (flags&FLAG_PREC) {
                if (sig_digits > 19) {
                        sig_digits = 19;
                } else if (sig_digits == 0) {
                        // always at least 1
                        sig_digits = 1;
                }
        } else {
                // no precision specified, taken as 6
                sig_digits = 6;
        }

        // sign
        if (s) {
                output_char(ctxt, '-');
        } else if (flags&FLAG_SIGN) {
                output_char(ctxt, '+');
        } else if (flags&FLAG_WSPC) {
                output_char(ctxt, ' ');
        }

        char n_array[20];
        int nb_digits = int_fmt_d(n_array, n);
        if (nb_digits == 0) {
                // floating point is zero
                output_char(ctxt, '0');
                --sig_digits;
                if (flags&(FLAG_ALTF)) {
                        output_char(ctxt, '.');
                        output_char_loop(ctxt, '0', sig_digits);
                }
                return;
        }

        int32_t exp_e = F+18;
        if (exp_e < -4 || exp_e >= sig_digits) {
                // %e-style conversion
                output_char(ctxt, n_array[0]);
                --sig_digits;
                int cur = sig_digits;
                if (!(flags&FLAG_ALTF)) {
                        // remove trailing zeros
                        while (n_array[cur] == '0') --cur;
                        if (cur > 0) {
                                output_char(ctxt, '.');
                        }
                } else {
                        output_char(ctxt, '.');
                }
                output_buffer(ctxt, n_array+1, cur);

                // write decimal exponent
                int32_t e_abs = (exp_e < 0) ? -exp_e : exp_e;
                char e_array[10];
                int e_digits = int_fmt_d(e_array, e_abs);
                char e_adjust = (flags|FLAG_UCAS) ? 'A' - 'a' : 0; // conditionally set to upper case
                output_char(ctxt, 'e' + e_adjust);
                if (exp_e < 0) {
                        output_char(ctxt, '-');
                } else {
                        output_char(ctxt, '+');
                }
                if (e_digits < 2) {
                        // write with at least 2 digits
                        output_char(ctxt, '0');
                        if (e_digits == 1) {
                                output_char(ctxt, e_array[0]);
                        } else {
                                // in other words, F == 0
                                output_char(ctxt, '0');
                        }
                } else {
                        output_buffer(ctxt, e_array, e_digits);
                }
                return;
        }

        // %f-style conversion
        int digits_before_point = F+19; // digits before the decimal point
        if (digits_before_point >= 19) {
                output_buffer(ctxt, n_array, 19);
                output_char_loop(ctxt, '0', digits_before_point-19);
                if (flags&FLAG_ALTF) {
                        output_char(ctxt, '.');
                }
        } else if (digits_before_point > 0) {
                int digits_left = nb_digits-digits_before_point;
                sig_digits -= digits_before_point;
                output_buffer(ctxt, n_array, digits_before_point);
                if (flags&FLAG_ALTF) {
                        output_char(ctxt, '.');
                        if (sig_digits <= 0) {
                                return; // already printed enough significant digits
                        }
                        if (digits_left >= sig_digits) {
                                output_buffer(ctxt, n_array+digits_before_point, sig_digits);
                        } else {
                                output_buffer(ctxt, n_array+digits_before_point, digits_left);
                                output_char_loop(ctxt, '0', sig_digits-digits_left);
                        }
                } else {
                        if (sig_digits <= 0) {
                                return; // already printed enough significant digits
                        }
                        // remove trailing zeros
                        int cur = nb_digits-1;
                        while (n_array[cur] == '0' && cur-- >= digits_before_point) {
                                --digits_left;
                        }
                        if (digits_left > 0) {
                                output_char(ctxt, '.');
                        }
                        if (digits_left >= sig_digits) {
                                output_buffer(ctxt, n_array+digits_before_point, sig_digits);
                        } else {
                                output_buffer(ctxt, n_array+digits_before_point, digits_left);
                        }
                }
        } else {
                output_char(ctxt, '0');
                output_char(ctxt, '.');
                output_char_loop(ctxt, '0', (size_t)(-digits_before_point));
                output_buffer(ctxt, n_array, sig_digits);
        }
}

void fp_special_case(bm_output_ctxt *ctxt, fpclass_t class) {
        uint16_t flags = ctxt->flags;
        char buff[3] = {'n', 'a', 'n'};
        if (class != BM_NAN) {
                buff[0] = 'i';
                buff[1] = 'n';
                buff[2] = 'f';
                if (class == BM_POS_INF) {
                        if (flags&FLAG_SIGN) {
                                output_char(ctxt, '+');
                        } else if (flags&FLAG_WSPC) {
                                output_char(ctxt, ' ');
                        }
                } else {
                        // class == BM_NEG_INF
                        output_char(ctxt, '-');
                }
        }
        if (ctxt->flags&FLAG_UCAS) {
                char maj = 'A' - 'a'; // upper case adjustement
                buff[0] += maj;
                buff[1] += maj;
                buff[2] += maj;
        }
        output_buffer(ctxt, buff, 3);
}
