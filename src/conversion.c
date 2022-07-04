#include "conversion.h"
#include "print.h"
#include "bipartite_t.h"



// functions to decompose floating point numbers into
// their constituent parts: sign, exponent, mantissa

fpclass_t decomposeDouble(char *s, int32_t *E, uint64_t *m, double x) {
        dblcst_t t;
        t.f = x;

        if (!ARE_SAME_ENDIANNESS_INT_FLT) {
                int l = 8; // IEEE-754 double: 64 bits = 8 bytes
                for (int i = 0; i < l/2; i++) {
                        SWAP_BYTES(((char *)(&t.i))[i], ((char *)(&t.i))[l-i-1])
                }
        }

        *s = t.i >> 63;
        *E = (t.i >> 52) & 0x7FF;
        *m = t.i & ((uint64_t)0xFFFFFFFFFFFFF);

        if (*E != 2047) {
                // x is a BM_NUMBER
                if (*E != 0) {
                        // x is normalized
                        *E = *E - 1023 - 52;
                        *m = *m + (((uint64_t)1) << 52); //implicit 1
                } else {
                        // binary exponent is zero: x is denormalized or zero
                        if (m != 0) {
                                *E = 1 - 1023 - 52;
                        }
                        // else *E == 0 and m == 0: zero
                }
                return BM_NUMBER;
        } else {
                // exponent == 2047: all ones
                if (!(*m)) {
                        // mantissa is zero: infinity
                        if (*s) {
                                return BM_NEG_INF;
                        }
                        return BM_POS_INF;
                }
                return BM_NAN;
        }
}

fpclass_t decomposeLongDouble(char *s, int32_t *E, uint64_t *m, long double x) {
        longdblcst_t t;
        t.f = x;

        if (!ARE_SAME_ENDIANNESS_INT_FLT) {
                int l = 10; // IEEE-754 80-bit extended: 80 bit = 10 bytes
                for (int i = 0; i < l/2; i++) {
                        SWAP_BYTES(((char *)(&t.i))[i], ((char *)(&t.i))[l-i-1])
                }
        }

        *s = (t.i >> 79) & 1;
        *E = (t.i >> 64) & 0x7FFF;
        *m = (uint64_t)t.i;

        if (*E != 32767) {
                // x is a BM_NUMBER
                if (*E != 0) {
                        // x is normalized
                        *E = *E - 16383 - 63;
                } else {
                        // binary exponent is zero: x is denormalized or zero
                        if (m != 0) {
                                *E = 1 - 16383 - 63;
                        }
                        // else *E == 0 and m == 0: zero
                }
                return BM_NUMBER;
        } else {
                // exponent == 32767: all ones
                if ((*m & (uint64_t)0x3FFFFFFFFFFFFFFF) == 0) {
                        // all 63 lower mantissa bits are 0
                        if (*s) {
                                return BM_NEG_INF;
                        }
                        return BM_POS_INF;
                }
                return BM_NAN;
        }
}




// functions to compute the decimal exponent and mantissa
// given the binary exponent and mantissa

static inline int64_t decimalExponent(int32_t E) {
        // 13 bits max
        int64_t constantLog = 330985980541; // floor(2^40*log10(2))
        return (((int64_t)E * constantLog) >> 40)+1;
}

static inline int32_t Delta(int64_t F) {
        // == floor(log2(5^(-F))), 14 bits max
        uint64_t constantLog = 2552986939188; // floor(log2(5)*2^40)
        return (int32_t)(((__int128)(-F)*constantLog) >> 40);
}

static inline int64_t Sigma(int32_t E, int64_t F, int32_t delta) {
        return (E-F-126+delta);
}

static inline int Tau(int64_t F, int Fh, int Fl) {
        return 2-Delta(F)+Delta(256*Fh)+Delta(Fl);
}

static inline void bipartiteT(int64_t F, uint64_t *tH, uint64_t *tL) {
        int Fh, Fl; // Fh = div(F,256), Fl = rem(F,256), and 0 <= rem < 256
        if (F >= 0) {
                Fh = F >> 8;
                Fl = F & 0xFF;
        } else {
                // careful with division && modulus with negative integers
                int32_t minusF = -F;
                int32_t quo = minusF >> 8;
                int32_t rem = minusF & 0xFF;
                Fh = -quo;
                Fl = -rem;
                if (rem != 0) {
                        --Fh;
                        Fl += 256;
                }
        }

        uint64_t t1H = t1_high[Fh+FH_BIAS];
        uint64_t t1L = t1_low[Fh+FH_BIAS];
        uint64_t t2H = t2_high[Fl];
        uint64_t t2L = t2_low[Fl];

        unsigned __int128 TL = (unsigned __int128)t2L*t1L;
        unsigned __int128 TM1 = (unsigned __int128)t1H*t2L;
        unsigned __int128 TM2 = (unsigned __int128)t1L*t2H;
        unsigned __int128 TH = (unsigned __int128)t2H*t1H;


        unsigned __int128 th, tm1, tm2;
        unsigned __int128 carry;
        uint64_t MASK_L64 = 0xFFFFFFFFFFFFFFFF; // for 64 low-order bits

        tm1 = (TM1 & MASK_L64) + (TM2 & MASK_L64) + (TL >> 64);
        carry = tm1 >> 64;
        tm1 &= MASK_L64;

        tm2 = (TH & MASK_L64) + (TM1 >> 64) + (TM2 >> 64) + carry;
        carry = tm2 >> 64;
        tm2 &= MASK_L64;

        th = (TH >> 64) + carry;


        int tau = Tau(F,Fh,Fl);
        th <<= tau;
        tm2 <<= tau;
        tm2 += tm1 >> (64-tau);
        carry = tm2 >> 64;
        tm2 -= carry << 64;
        th += carry;

        // t = tH*2^(64)+tL
        *tH = (uint64_t)th;
        *tL = (uint64_t)tm2;
}

static inline void multiply_mt(int64_t F, uint64_t m, unsigned __int128 *nH, unsigned __int128 *nM, unsigned __int128 *nL) {
        // returns m*t
        unsigned __int128 cin;
        uint64_t tH, tL;

        bipartiteT(F, &tH, &tL);

        *nL = (unsigned __int128)m*tL;
        *nM = (unsigned __int128)m*tH;

        *nH = *nM >> 64;
        *nM &= ((unsigned __int128)1 << 64)-1; // passing on 64 highest bits to nH
        *nM += *nL >> 64;
        *nL &= ((unsigned __int128)1 << 64)-1; // passing on 64 highest bits to nM

        cin = *nM >> 64;
        *nH += cin;
        *nM -= cin << 64;
}


static inline uint64_t decimalMantissa(int64_t F, uint64_t m, int32_t E) {
        int sigma;
        uint32_t shift;

        sigma = Sigma(E,F,Delta(F)); // -130 <= sigma <= -127

        shift = (uint32_t)(-sigma);
        unsigned __int128 binary = ((unsigned __int128)1) << (shift-65);
        unsigned __int128 nH, nM, nL, cin;

        multiply_mt(F, m, &nH, &nM, &nL);
        nM += binary;
        cin = nM >> 64;
        nH += cin;

        if (shift == 127) {
                nM >>= (shift-64);
                nH <<= (128-shift);
                nH += nM;
        } else {
                nH >>= (shift-128);
        }
        return (uint64_t)nH;
}

static inline void adjust_m(int32_t *E, uint64_t *m) {
        // we want 2^63 <= m < 2^64
        while (*m < (((uint64_t)1) << 63)) {
                *m <<= 1;
                *E -= 1;
        }
}

void decimalConversion(int32_t *F, uint64_t *n, int32_t E, uint64_t m) {
        // supposes the floating point is of type BM_NUMBER
        if (m != 0) {
                adjust_m(&E, &m);
                *F = decimalExponent(E);
                *n = decimalMantissa(*F, m, E);
        } else {
                // m == 0 indicates that the number is zero
                *n = 0;
                *F = 0;
        }
}
