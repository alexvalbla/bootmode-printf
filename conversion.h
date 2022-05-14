#ifndef BM_CONVERSION_H
#define BM_CONVERSION_H


#include "bm_utils.h"

typedef union{
  float f;
  uint32_t i;
} fltcst_t;

typedef union{
  double f;
  uint64_t i;
} dblcst_t;

typedef union{
  long double f;
  __uint128_t i;
} longdblcst_t;

typedef enum {BM_NUMBER = 0, BM_NAN = 1, BM_POS_INF = 2, BM_NEG_INF = 3} fpclass_t;


fpclass_t decomposeDouble(char *s, int32_t *E, uint64_t *m, double x);

fpclass_t decomposeLongDouble(char *s, int32_t *E, uint64_t *m, long double x);

void decimalConversion(int32_t *F, uint64_t *n, int32_t E, uint64_t m);


#endif // conversion.h
