#include "conversion.h"
#include "print.h"
#include "tableT1.h"
#include "tableT2.h"


// little macro to help correct endianness
#define SWAP_BYTES(b1,b2)\
        (b1) ^= (b2);\
        (b2) ^= (b1);\
        (b1) ^= (b2);

// functions to decompose floating point numbers into
// their constituent parts: sign, exponent, mantissa

fpclass_t decomposeDouble(char *s, int32_t *E, uint64_t *m, double x) {
        dblcst_t t;
        t.f = x;

        if (!ARE_SAME_ENDIANNESS_INT_FLT) {
                SWAP_BYTES(((char *)(&t.i))[0], ((char *)(&t.i))[7])
                SWAP_BYTES(((char *)(&t.i))[1], ((char *)(&t.i))[6])
                SWAP_BYTES(((char *)(&t.i))[2], ((char *)(&t.i))[5])
                SWAP_BYTES(((char *)(&t.i))[3], ((char *)(&t.i))[4])
        }

        *s = t.i >> 63;
        *E = (t.i >> 52) & 0x7FF;
        *m = t.i & ((uint64_t)0xFFFFFFFFFFFFF);

        if (*E != 2047) {
                //x is a BM_NUMBER
                if (*E != 0) {
                        //x is normalized
                        *E = *E - 1023 - 52;
                        *m = *m + (((uint64_t)1)<<52); //implicit 1
                } else {
                        //binary exponent is zero: x is denormalized or zero
                        if (m != 0) {
                                *E = 1 - 1023 - 52;
                        }
                        //else *E == 0 and m == 0: zero
                }
                return BM_NUMBER;
        } else {
                //exponent == 2047: all ones
                if (!(*m)) {
                        //mantissa is zero: infinity
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
        *s = t.i >> 79;
        *E = (t.i >> 64) & 0x7FFF;
        *m = (uint64_t)t.i;

        if (*E != 32767) {
                //x is a BM_NUMBER
                if (*E != 0) {
                        //x is normalized
                        *E = *E - 16383 - 63;
                } else {
                        //binary exponent is zero: x is denormalized or zero
                        if (m != 0) {
                                *E = 1 - 16383 - 63;
                        }
                        //else *E == 0 and m == 0: zero
                }
                return BM_NUMBER;
        }
        else {
                //exponent == 32767: all ones
                if ((*m >> 62) == 0) {
                        //case bits 63 and 62 are 00
                        if (!(*m)) {
                                //case all other mantissa bits are also 0, means infinity
                                if (*s) {
                                        return BM_NEG_INF;
                                }
                                return BM_POS_INF;
                        }
                        return BM_NAN;//case not all other mantissa bits are 0
                }

                else if ((*m >> 62) == 1) {
                        //case bits 63 and 62 are 01
                        return BM_NAN;
                }

                else if ((*m >> 62) == 2) {
                        //case bits 63 and 62 are 10
                        if ((*m << 2) == 0) {
                                //case all other mantissa bits are also 0, means infinity
                                if (*s) {
                                        return BM_NEG_INF;
                                }
                                return BM_POS_INF;
                        }
                        return BM_NAN;//case other mantissa bits are not all 0
                }

                else {
                        //case bits 63 and 62 are 11
                        return BM_NAN;//Actually floating point indefinite or NaN, will be interpreted as NaN
                }
        }
}




//functions to compute the decimal exponent and mantissa
//given the binary exponent and mantissa

static inline int64_t decimalExponent(int32_t E) {
        // 13 bits max
        int64_t constantLog = 330985980541; //floor(2^40*log10(2))
        return (((int64_t)E * constantLog) >> 40)+1;
}

static inline int32_t Delta(int64_t F) {
        // = floor(log2(5^(-F))), 14 bits max
        uint64_t constantLog = 2552986939188; //floor(log2(5)*2^40)
        return (int32_t)(((__int128_t)(-F)*constantLog) >> 40);
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

        uint64_t t1H = t1High[Fh+Fhbias];
        uint64_t t1L = t1Low[Fh+Fhbias];
        uint64_t t2H = t2High[Fl];
        uint64_t t2L = t2Low[Fl];

        __uint128_t TL = (__uint128_t)t2L*t1L;
        __uint128_t TM1 = (__uint128_t)t1H*t2L;
        __uint128_t TM2 = (__uint128_t)t1L*t2H;
        __uint128_t TH = (__uint128_t)t2H*t1H;


        __uint128_t th, tm1, tm2;
        __uint128_t carry;
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

        //t = tH*2^(64)+tL
        *tH = (uint64_t)th;
        *tL = (uint64_t)tm2;
}

static inline void multiply_mt(int64_t F, uint64_t m, __uint128_t *nH, __uint128_t *nM, __uint128_t *nL) {
        // returns m*t
        __uint128_t cin;
        uint64_t tH, tL;

        bipartiteT(F, &tH, &tL);

        *nL = (__uint128_t)m*tL;
        *nM = (__uint128_t)m*tH;

        *nH = *nM>>64;
        *nM &= ((__uint128_t)1<<64)-1;// passing on 64 highest bits to nH
        *nM += *nL>>64;
        *nL &= ((__uint128_t)1<<64)-1;// passing on 64 highest bits to nM

        cin = *nM>>64;
        *nH += cin;
        *nM -= cin<<64;
}


static inline uint64_t decimalMantissa(int64_t F, uint64_t m, int32_t E) {
        int sigma;
        uint32_t shift;

        sigma = Sigma(E,F,Delta(F)); //-130<= sigma<= -127

        shift = (uint32_t)(-sigma);
        __uint128_t binary = ((__uint128_t)1) << (shift-65);
        __uint128_t nH, nM, nL, cin;

        multiply_mt(F, m, &nH, &nM, &nL);
        nM += binary;
        cin = nM>>64;
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
        //we want 2^63 <= m < 2^64decimalMantissa
        if (*m < (((uint64_t)1) << 63)) {
                if (*m < (((uint64_t)1) << 32)) {
                        *m <<= 32;
                        *E -= 32;
                }
                if (*m < (((uint64_t)1) << 48)) {
                        *m <<= 16;
                        *E -= 16;
                }
                if (*m < (((uint64_t)1) << 56)) {
                        *m <<= 8;
                        *E -= 8;
                }
                if (*m < (((uint64_t)1) << 60)) {
                        *m <<= 4;
                        *E -= 4;
                }
                if (*m < (((uint64_t)1) << 63)) {
                        *m <<= 2;
                        *E -= 2;
                }
                if (*m < (((uint64_t)1) << 63)) {
                        *m <<= 1;
                        *E -= 1;
                }
        }
}

static inline void adjust_n_and_F(uint64_t *n, int32_t *F) {
        //we want 10^18 <= n < 10^19
        if (*n >= (uint64_t)10000000000000000000u) {
                //n >= 10^19
                int r = (*n)%10;
                *n /= 10;
                (*F)++;
                if (r >= 5) {
                        //the last digit was >= 5
                        //the round up
                        (*n)++;
                        if (*n >= (uint64_t)10000000000000000000u) {
                                //if by rounding we again have n >= 10^19
                                //we again impose n < 10^19
                                *n /= 10;
                                (*F)++;
                        }
                }
        } else if (*n < (uint64_t)1000000000000000000u) {
                //n < 10^18
                *n *= 10;
                (*F)--;
        }
}

void decimalConversion(int32_t *F, uint64_t *n, int32_t E, uint64_t m) {
        // supposes the floating point is of type BM_NUMBER
        if (m != 0) {
                adjust_m(&E, &m);
                *F = decimalExponent(E);
                *n = decimalMantissa(*F, m, E);
                adjust_n_and_F(n, F);
        } else {
                // m == 0 indicates that the number is zero
                *n = 0;
                *F = 0;
        }
}
