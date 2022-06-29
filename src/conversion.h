#ifndef BM_CONVERSION_H
#define BM_CONVERSION_H


#include "print.h"

// little macro to help correct endianness
#define SWAP_BYTES(b1,b2)\
        (b1) ^= (b2);\
        (b2) ^= (b1);\
        (b1) ^= (b2);

typedef union {
  double f;
  uint64_t i;
} dblcst_t;

typedef union {
  long double f;
  unsigned __int128 i;
} longdblcst_t;

typedef enum {BM_NUMBER, BM_NAN, BM_POS_INF, BM_NEG_INF} fpclass_t;


fpclass_t decomposeDouble(char *s, int32_t *E, uint64_t *m, double x);

fpclass_t decomposeLongDouble(char *s, int32_t *E, uint64_t *m, long double x);

void decimalConversion(int32_t *F, uint64_t *n, int32_t E, uint64_t m);


#endif // conversion.h
