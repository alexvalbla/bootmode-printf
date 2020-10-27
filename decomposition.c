#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decomposition.h"






fpclass_t decomposeDouble(int *s, int32_t *E, uint64_t *m, double x){
  dblcst_t t;
  t.f = x;
  *s = 1&(t.i>>63);
  *E = (t.i>>52)&((((int32_t)1)<<11)-1);
  *m = t.i&((((uint64_t)1)<<52)-1);

  if((*E) != 2047){
    //must be a NUMBER
    if(*E != 0){
      //normalized value
      *E = *E - 1023 - 52;
      *m = *m + ((uint64_t)1<<52);//implicit 1
    }
    else{
      //Exponent is all zeros : denormalized value or zero
      if(m != 0) *E = 1 - 1023 - 52;//case of denormalized
      //otherwise E == 0 and m == 0 : zero
    }
    return NUMBER;
  }
  else{
    //Exponent = 2047 : all ones
    if(!(*m)){
      //mantissa all zeros
      if((*s) == 1) return NEG_INF;
      return POS_INF;
    }
    return NAN;
  }
}




fpclass_t decomposeLongDouble(int *s, int32_t *E, uint64_t *m, long double x){
  longdblcst_t t;
  t.f = x;
  *s = 1&(t.i>>79);
  *E = (int32_t)((t.i >> 64)&((((__uint128_t)1)<<15) -1));
  *m = (uint64_t)(t.i & (((__uint128_t)1 << 64) - 1));

  if((*E) != 32767){
    //must be a NUMBER
    if((*E) != 0){
      //normalized value
      *E = *E - 16383 - 63;
    }
    else{
     
      if(m != 0){
        *E = 1 - 16383 - 63;
      }
      //else E==0 and m==0 is zero
    }
    return NUMBER;
  }

  else{
    //Exponent == 32767 : all ones
    if((*m >> 62) == 0){
      //case bits 63 and 62 are 00
      if((*m) == 0){
        //case all other mantissa bits are also 0, means infinity
        if(*s == 1) return NEG_INF;
        return POS_INF;
      }
      return NAN;//case not all other mantissa bits are 0
    }

    else if((*m >>62) == 1){
      //case bits 63 and 62 are 01
      return NAN;
    }

    else if((*m >>62) == 2){
      //case bits 63 and 62 are 10
      if((*m<<2) == 0){
        //case all other mantissa bits are also 0, means infinity
        if(*s == 1) return NEG_INF;
        return POS_INF;
      }
      return NAN;//case other mantissa bits are not all 0
    }

    else{
      //case bits 63 and 62 are 11
      return NAN;//Actually floating point indefinite or NaN, will be interpreted as NaN
    }
  }
 
}
