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
        char conv_buff[45]; // 128-bit integer as octal is at most 43 character + extra '0' for %#o + '\0'
        char prefix[2];
        unsigned int prefix_len = 0;
        unsigned int nb_digits = 0;
        uintmax_t mask;
        switch (ctxt->specifier) {
                case 'X':
                case 'x':
                        ctxt->base = 16;
                        if ((ctxt->flags&FLAG_ALTF) && a > 0) {
                                prefix[prefix_len++] = '0';
                                prefix[prefix_len++] = ctxt->specifier; // either 'x' or 'X'
                        }
                        break;
                case 'o':
                        ctxt->base = 8;
                        if (a != 0 && (ctxt->flags&FLAG_ALTF)) {
                                // first digit must be 0
                                conv_buff[nb_digits++] = '0';
                        }
                        break;
                case 'i':
                case 'd':
                        arg_sz <<= 3; // x8 to get size in bits
                        mask = ((uintmax_t)1) << (arg_sz-1); // aim at sign bit
                        if (a & mask) {
                                // sign bit is set, argument was negative
                                // 2's complement representation of argument...
                                // ... as an integer of size uintmax_t:
                                a |= ~(mask-1); // <- all bits after sign bit must be set to 1
                                a = ~(a-1); // <- 2's complement for absolute value
                                prefix[prefix_len++] = '-';
                        } else if (ctxt->flags&FLAG_SIGN) {
                                prefix[prefix_len++] = '+';
                        } else if (ctxt->flags&FLAG_WSPC) {
                                prefix[prefix_len++] = ' ';
                        } else {
                                prefix_len = 0; // no sign character
                        }
                        // fall through
                default:
                        ctxt->base = 10; // for 'i', 'd', and 'u'
                        break;
        }
        nb_digits += int_fmt(ctxt, conv_buff+nb_digits, a);

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
        }

        // pad then output conversion:
        uint16_t flags = ctxt->flags;
        unsigned int field_width = ctxt->field_width;
        unsigned int precision = ctxt->precision;

        unsigned int prec_padding = (nb_digits >= precision) ? 0 : precision - nb_digits;
        unsigned int total_length = nb_digits + prec_padding + prefix_len;
        unsigned int padding_length = 0;
        if (flags&FLAG_WDTH && total_length < field_width) {
                padding_length = field_width - total_length;
        }

        if (flags&FLAG_ZERO) {
                output_buffer(ctxt, prefix, prefix_len);
                output_char_loop(ctxt, '0', prec_padding+padding_length);
                output_buffer(ctxt, conv_buff, nb_digits);
        } else {
                if (!(flags&FLAG_LADJ)) {
                        output_char_loop(ctxt, ' ', padding_length);
                }
                output_buffer(ctxt, prefix, prefix_len);
                output_char_loop(ctxt, '0', prec_padding);
                output_buffer(ctxt, conv_buff, nb_digits);
                if (flags&FLAG_LADJ) {
                        output_char_loop(ctxt, ' ', padding_length);
                }
        }
}

void output_str(bm_output_ctxt *ctxt, bm_va_list ap) {
        uint16_t flags = ctxt->flags;
        unsigned int field_width = ctxt->field_width;
        unsigned int precision = ctxt->precision;
        size_t len;
        const char *s;
        char c;
        if (ctxt->specifier == 'c') {
                c = (char)bm_va_arg(ap, int);
                s = &c;
                len = 1;
        } else {
                // ctxt->specifier == 's'
                s = bm_va_arg(ap, const char *);
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
                        // no more than 'precision' characters should be printed
                        len = precision;
                }
        }

        // pad and output
        size_t padding_length = 0;
        if (flags&FLAG_WDTH && len < field_width) {
                padding_length = field_width-len;
        }

        if (!(flags&FLAG_LADJ)) {
                output_char_loop(ctxt, ' ', padding_length);
        }
        output_buffer(ctxt, s, len);
        if (flags&FLAG_LADJ) {
                output_char_loop(ctxt, ' ', padding_length);
        }
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


// floating point formatting functions

static void fp_fmt_e(bm_output_ctxt *ctxt, char *digits, unsigned int nb_digits, int32_t F);
static void fp_fmt_f(bm_output_ctxt *ctxt, char *digits, unsigned int nb_digits, int position);
static void fp_fmt_g(bm_output_ctxt *ctxt, char *digits, unsigned int nb_digits, int32_t F);
static void fp_special_case(bm_output_ctxt *ctxt, fpclass_t class);

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
        if (s != 0) {
                // sign bit is set
                ctxt->flags |= FLAG_NEGV;
        }
        if (class == BM_NUMBER) {
                ctxt->base = 10;
                decimalConversion(&F, &n, E, m);
                if (n == 0) {
                        // float value is 0.
                        ctxt->flags |= FLAG_FLT0;
                }
                if (!(ctxt->flags&FLAG_PREC)) {
                        // precision missing, taken as 6
                        ctxt->precision = 6;
                }
                char digits[32];
                unsigned int nb_digits = int_fmt(ctxt, digits, n);
                switch (ctxt->specifier) {
                        case 'e':
                                F += nb_digits-1;
                                fp_fmt_e(ctxt, digits, nb_digits, F);
                                break;
                        case 'f':
                                fp_fmt_f(ctxt, digits, nb_digits, F+nb_digits);
                                break;
                        case 'g':
                                fp_fmt_g(ctxt, digits, nb_digits, F);
                                // fp_fmt_g(ctxt, s, n, F);
                                break;
                        default:
                                break;
                }
        } else {
                // otherwise: +inf, -inf or nan
                fp_special_case(ctxt, class);
        }
}

static void fp_fmt_e(bm_output_ctxt *ctxt, char *digits, unsigned int nb_digits, int32_t F) {
        uint16_t flags = ctxt->flags;
        unsigned int field_width = ctxt->field_width;
        unsigned int precision = ctxt->precision;

        char sign_char = 0;
        if (flags&FLAG_NEGV) {
                sign_char = '-';
        } else if (flags&FLAG_SIGN) {
                sign_char = '+';
        } else if (flags&FLAG_WSPC) {
                sign_char = ' ';
        }

        unsigned int nb_digits_F;
        char digits_F[5];
        char F_sign_char = '+';
        char E = (flags&FLAG_UCAS) ? 'E' : 'e';
        if (flags&FLAG_FLT0) {
                // f.p. is 0.
                nb_digits = 1;
                nb_digits_F = 1;
                digits[0] = '0';
                digits_F[0] = '0';
        } else {
                // f.p. is not 0. -> nb_digits > 0
                if (F == 0) {
                        nb_digits_F = 1;
                        digits_F[0] = '0';
                } else {
                        if (F < 0) {
                                F_sign_char = '-';
                                F = -F;
                        }
                        nb_digits_F = int_fmt(ctxt, digits_F, F);
                }
        }
        // digits[0] is consumed as the first digit before decimal point
        --nb_digits;

        unsigned int digits_after_point = (precision <= nb_digits) ? precision : nb_digits;
        unsigned int trailing_zeros;
        if ((flags&FLAG_REM0) || precision <= nb_digits) {
                trailing_zeros = 0;
        } else {
                trailing_zeros = precision - nb_digits;
        }

        char point = 0;
        if ((flags&FLAG_ALTF) || digits_after_point + trailing_zeros > 0) {
                point = '.';
        }

        if (nb_digits_F == 1) {
                // should be printed with at least 2 digits
                digits_F[1] = digits_F[0];
                digits_F[0] = '0';
                ++nb_digits_F;
        }

        // total length explanation:
        // - 1 digits before decimal point
        // - X digits after decimal point
        // - Y trailing zeros
        // - 'e' character followed by '+' or '-'
        // - Z decimal exponent digits
        // - maybe a sign character
        // - maybe a decimal point
        unsigned int total_length = 1 + digits_after_point + trailing_zeros + 2 + nb_digits_F;
        if (sign_char) ++total_length;
        if (point) ++total_length;

        unsigned int padding_length = 0;
        if ((flags&FLAG_WDTH) && total_length < field_width) {
                padding_length = field_width - total_length;
        }

        if (!(flags&FLAG_LADJ)) {
                if (flags|FLAG_ZERO) {
                        if (sign_char) output_char(ctxt, sign_char);
                        output_char_loop (ctxt, '0', padding_length);
                } else {
                        output_char_loop (ctxt, ' ', padding_length);
                        if (sign_char) output_char(ctxt, sign_char);
                }
        } else if (sign_char) output_char(ctxt, sign_char);
        output_char(ctxt, digits[0]);
        if (point) output_char(ctxt, point);
        output_buffer(ctxt, digits+1, digits_after_point);
        output_char_loop(ctxt, '0', trailing_zeros);
        output_char(ctxt, E);
        output_char(ctxt, F_sign_char);
        output_buffer(ctxt, digits_F, nb_digits_F);
        if (flags&FLAG_LADJ) {
                output_char_loop (ctxt, ' ', padding_length);
        }
}

static void fp_fmt_f(bm_output_ctxt *ctxt, char *digits, unsigned int nb_digits, int position) {
        uint16_t flags = ctxt->flags;
        unsigned int field_width = ctxt->field_width;
        unsigned int precision = ctxt->precision;

        char sign_char = 0;
        if (flags&FLAG_NEGV) {
                sign_char = '-';
        } else if (flags&FLAG_SIGN) {
                sign_char = '+';
        } else if (flags&FLAG_WSPC) {
                sign_char = ' ';
        }

        if (flags&FLAG_FLT0) {
                nb_digits = 0;
                position = 0;
        }

        unsigned int digits_before_point;
        unsigned int zeros_before_point;
        unsigned int zeros_after_point;
        unsigned int digits_after_point;
        unsigned int trailing_zeros;

        if (position > 0) {
                if (position >= nb_digits) {
                        digits_before_point = nb_digits;
                        zeros_before_point = position - nb_digits;
                        nb_digits = 0; // all are consumed before decimal point
                } else {
                        digits_before_point = position;
                        zeros_before_point = 0;
                        nb_digits -= position;
                }
                zeros_after_point = 0;
                digits_after_point = (precision >= nb_digits) ? nb_digits : precision;
        } else {
                // position <= 0
                unsigned int neg_pos = -position;
                digits_before_point = 0;
                zeros_before_point = 1;
                zeros_after_point = (neg_pos >= precision) ? (flags&FLAG_REM0 ? 0 : precision) : neg_pos;
                digits_after_point = (neg_pos >= precision) ? 0 : (nb_digits >= precision - zeros_after_point) ? precision - zeros_after_point : nb_digits;
        }
        trailing_zeros = (flags&FLAG_REM0) ? 0 : precision - zeros_after_point - digits_after_point;

        char point = 0;
        if ((flags&FLAG_ALTF) || (zeros_after_point|digits_after_point|trailing_zeros) != 0) {
                point = '.';
        }

        unsigned int total_length = digits_before_point
                                  + zeros_before_point
                                  + zeros_after_point
                                  + digits_after_point
                                  + trailing_zeros;
        if (sign_char) ++total_length;
        if (point) ++total_length;

        unsigned int padding_length = 0;
        if ((flags&FLAG_WDTH) && total_length < field_width) {
                padding_length = field_width - total_length;
        }

        if (!(flags&FLAG_LADJ)) {
                if (flags|FLAG_ZERO) {
                        if (sign_char) output_char(ctxt, sign_char);
                        output_char_loop (ctxt, '0', padding_length);
                } else {
                        output_char_loop (ctxt, ' ', padding_length);
                        if (sign_char) output_char(ctxt, sign_char);
                }
        } else if (sign_char) output_char(ctxt, sign_char);

        output_buffer(ctxt, digits, digits_before_point);
        output_char_loop(ctxt, '0', zeros_before_point);
        if (point) output_char(ctxt, point);
        output_char_loop(ctxt, '0', zeros_after_point);
        output_buffer(ctxt, &digits[digits_before_point], digits_after_point);
        output_char_loop(ctxt, '0', trailing_zeros);
        if (flags&FLAG_LADJ) {
                output_char_loop (ctxt, ' ', padding_length);
        }
}

static void fp_fmt_g(bm_output_ctxt *ctxt, char *digits, unsigned int nb_digits, int32_t F) {
        if (ctxt->precision == 0) ctxt->precision = 1;
        if (!(ctxt->flags&FLAG_ALTF)) {
                // remove trailing zeros
                ctxt->flags |= FLAG_REM0;
        }
        int32_t exp_e = F + (int)nb_digits -1;
        if (ctxt->flags&FLAG_FLT0 || (exp_e >= -4 && exp_e < (int)ctxt->precision)) {
                int position = exp_e + 1;
                if (position > 0) ctxt->precision -= position;
                else if (ctxt->flags&FLAG_FLT0) --(ctxt->precision);
                fp_fmt_f(ctxt, digits, nb_digits, position);
        } else {
                --(ctxt->precision);
                fp_fmt_e(ctxt, digits, nb_digits, exp_e);
        }
}

static void fp_special_case(bm_output_ctxt *ctxt, fpclass_t class) {
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
