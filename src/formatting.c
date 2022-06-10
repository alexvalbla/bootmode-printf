#include "formatting.h"
#include "conversion.h"
#include "output_utils.h"



static void str_rev(char *s, size_t size) {
        char *p = s + size;
        --p;
        while (s < p) {
                SWAP_BYTES(*s, *p)
                ++s;
                --p;
        }
}

static int int_fmt(bm_output_ctxt *ctxt, char *conv_buff, uintmax_t a) {
        int i = 0;
        while (a > 0) {
                int digit = a%ctxt->base;
                conv_buff[i++] = (digit < 10) ? digit+'0' : (ctxt->flags&FLAG_UCAS) ? digit-10+'A' : digit-10+'a';
                a /= ctxt->base;
        }
        str_rev(conv_buff, i);
        conv_buff[i] = '\0';
        return i;
}

void output_int(bm_output_ctxt *ctxt, bm_va_list ap) {
        // extract integer argument:
        uintmax_t a;
        size_t arg_sz;
        char *lmods = ctxt->lmods;
        if (ctxt->specifier == 'p') {
                a = (uintmax_t)bm_va_arg(ap, void *);
                arg_sz = sizeof(void *);
                // %p equivalent to %#x or %#lx:
                ctxt->flags |= FLAG_ALTF;
                ctxt->specifier = 'x';
        } else if (lmods[0] == 'l' && lmods[1] == '\0') {
                a = bm_va_arg(ap, unsigned long);
                arg_sz = sizeof(unsigned long);
        } else if (lmods[0] == 'l' && lmods[1] == 'l') {
                a = bm_va_arg(ap, unsigned long long);
                arg_sz = sizeof(unsigned long long);
        } else if (lmods[0] == 'h' && lmods[1] == '\0') {
                a = bm_va_arg(ap, unsigned int);
                arg_sz = sizeof(unsigned int);
        } else if (lmods[0] == 'h' && lmods[1] == 'h') {
                a = bm_va_arg(ap, unsigned int);
                arg_sz = sizeof(unsigned int);
        } else if (lmods[0] == 'z' && lmods[1] == '\0') {
                a = bm_va_arg(ap, size_t);
                arg_sz = sizeof(size_t);
        } else if (lmods[0] == 't' && lmods[1] == '\0') {
                a = bm_va_arg(ap, ptrdiff_t);
                arg_sz = sizeof(ptrdiff_t);
        } else if (lmods[0] == 'j' && lmods[1] == '\0') {
                a = bm_va_arg(ap, uintmax_t);
                arg_sz = sizeof(uintmax_t);
        } else {
                a = bm_va_arg(ap, unsigned int);
                arg_sz = sizeof(unsigned int);
        }

        // convert argument to digits:
        char conv_buff[32];
        int nb_digits = 0;
        uintmax_t mask;
        char prefix[2];
        char prefix_len = 0; // provisionally
        switch (ctxt->specifier) {
                case 'X':
                case 'x':
                        ctxt->base = 16;
                        if ((ctxt->flags&FLAG_ALTF) && a > 0) {
                                prefix_len = 2;
                                prefix[0] = '0';
                                prefix[1] = ctxt->specifier; // either 'x' or 'X'
                        }
                        break;
                case 'o':
                        ctxt->base = 8;
                        break;
                case 'i':
                case 'd':
                        prefix_len = 1; // provisionally, for sign character
                        arg_sz <<= 3; // x8 to get size in bits
                        mask = ((uintmax_t)1) << (arg_sz-1); // aim at sign bit
                        if (a & mask) {
                                // sign bit is set, argument was negative
                                // 2's complement representation of argument...
                                // ... as an integer of size uintmax_t:
                                a |= ~(mask-1); // <- all bits after sign bit must be set to 1
                                a = ~(a-1); // <- 2's complement for absolute value
                                prefix[0] = '-';
                        } else if (ctxt->flags&FLAG_SIGN) {
                                prefix[0] = '+';
                        } else if (ctxt->flags&FLAG_WSPC) {
                                prefix[0] = ' ';
                        } else {
                                prefix_len = 0; // no sign character
                        }
                        // fall through
                default:
                        ctxt->base = 10; // for 'i', 'd', and 'u'
                        break;
        }
        nb_digits = int_fmt(ctxt, conv_buff, a);

        // postprocessing:
        if (ctxt->flags&FLAG_PREC) {
                ctxt->flags &= ~((uint16_t)FLAG_ZERO); // precision specified, remove 0 flag
        } else {
                ctxt->precision = 1; // precision unspecified, taken to be 1
        }
        if (a == 0 && ctxt->precision > 0) {
                // precision > 0 but a == 0 -> print at least 1 digit
                conv_buff[0] = '0';
                nb_digits += 1;
        } else if (ctxt->specifier == 'o') {
                if ((ctxt->flags&FLAG_ALTF) && conv_buff[0] != '0') {
                        // first digit must be 0
                        int n = nb_digits;
                        while (n > 0) {
                                conv_buff[n] = conv_buff[n-1];
                                --n;
                        }
                        conv_buff[0] = '0';
                        ++nb_digits;
                }
        }

        // padd then output conversion:
        uint16_t flags = ctxt->flags;
        uint16_t field_width = ctxt->field_width;
        uint16_t precision = ctxt->precision;

        int16_t prec_padding = (nb_digits >= precision) ? 0 : precision - nb_digits;
        int16_t total_length = nb_digits + prec_padding + prefix_len;
        int padding_length = 0;
        if (flags&FLAG_WDTH && total_length < field_width) {
                padding_length = field_width - total_length;
        }

        if (flags&FLAG_ZERO) {
                if (prefix_len > 0) {
                        output_buffer(ctxt, prefix, prefix_len);
                }
                output_char_loop(ctxt, '0', prec_padding+padding_length);
                output_buffer(ctxt, conv_buff, nb_digits);
        } else {
                if (!(flags&FLAG_LADJ)) {
                        output_char_loop(ctxt, ' ', padding_length);
                }
                if (prefix_len > 0) {
                        output_buffer(ctxt, prefix, prefix_len);
                }
                output_char_loop(ctxt, '0', prec_padding);
                output_buffer(ctxt, conv_buff, nb_digits);
                if (flags&FLAG_LADJ) {
                        output_char_loop(ctxt, ' ', padding_length);
                }
        }
}

static void pad_and_output_str(bm_output_ctxt *ctxt, const char *str, size_t len) {
        uint16_t flags = ctxt->flags;
        uint16_t field_width = ctxt->field_width;

        if ((flags&FLAG_WDTH) && len < field_width) {
                size_t padding_length = field_width-len;
                if (flags&FLAG_LADJ) {
                        output_buffer(ctxt, str, len);
                        output_char_loop(ctxt, ' ', padding_length);
                } else {
                        output_char_loop(ctxt, ' ', padding_length);
                        output_buffer(ctxt, str, len);
                }
                return;
        }
        output_buffer(ctxt, str, len);
}

void output_c(bm_output_ctxt *ctxt, bm_va_list ap) {
        char c = (char)bm_va_arg(ap, int);
        pad_and_output_str(ctxt, (const char *)&c, 1);
}

void output_s(bm_output_ctxt *ctxt, bm_va_list ap) {
        const char *s = bm_va_arg(ap, const char *);
        uint16_t flags = ctxt->flags;
        uint16_t precision = ctxt->precision;
        size_t len;
        if (s == NULL) {
                s = "(null)";
                len = 6;
        } else {
                const char *cursor = s;
                while (*cursor) {
                        ++cursor;
                }
                len = cursor-s;
        }
        if ((flags&FLAG_PREC) && precision < len) {
                len = precision;
        }

        pad_and_output_str(ctxt, s, len);
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
        char s; // sign
        int32_t E; // binary exponent
        uint64_t m; // binary mantissa
        int32_t F; // decimal exponent
        uint64_t n; // decimal mantissa
        fpclass_t class;
        char *lmods = &(ctxt->lmods[0]);
        if (lmods[0] == 'L' && lmods[1] == '\0') {
                class = decomposeLongDouble(&s, &E, &m, bm_va_arg(ap, long double));
        } else {
                class = decomposeDouble(&s, &E, &m, bm_va_arg(ap, double));
        }
        if (!(ctxt->flags&FLAG_PREC)) {
                // precision missing, taken as 6:
                ctxt->precision = 6;
        }
        if (class == BM_NUMBER) {
                ctxt->base = 10;
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
        if (precision > 18) {
                precision = 18;
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
                int_fmt(ctxt, n_array, n);
                F += 18;
        }
        output_char(ctxt, n_array[0]);
        if (precision || (flags&FLAG_ALTF)) {
                output_char(ctxt, '.');
                output_buffer(ctxt, n_array+1, precision);
        }

        int32_t F_abs_val = (F < 0) ? -F : F;
        char F_array[10];
        int F_digits = int_fmt(ctxt, F_array, F_abs_val);

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
        if (precision > 19) {
                precision = 19;
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
                int_fmt(ctxt, n_array, n);
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
        if (sig_digits > 19) {
                sig_digits = 19;
        } else if (sig_digits == 0) {
                // always at least 1
                sig_digits = 1;
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
        int nb_digits = int_fmt(ctxt, n_array, n);
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
                int e_digits = int_fmt(ctxt, e_array, e_abs);
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
