#include <stdint.h>
#include <stdio.h>
#include "conversion.h"
#include "tableT1.h"
#include "tableT2.h"




//functions to decompose floatinf point numbers into
//their constituent parts: sign, exponent, mantissa

fpclass_t decomposeDouble(int *s, int32_t *E, uint64_t *m, double x){
  dblcst_t t;
  t.f = x;
  *s = (t.i>>63) & 1;
  *E = (t.i>>52) & ((((uint32_t)1)<<11)-1);
  *m = t.i & ((((uint64_t)1)<<52)-1);

  if(*E != 2047){
    //x is a NUMBER
    if(*E != 0){
      //x is normalized
      *E = *E - 1023 - 52;
      *m = *m + ((uint64_t)1<<52); //implicit 1
    }
    else{
      //binary exponent is zero: x is denormalized or zero
      if(m != 0){
        *E = 1 - 1023 - 52;
      }
      //else *E == 0 and m == 0: zero
    }
    return NUMBER;
  }
  else{
    //exponent == 2047: all ones
    if(!(*m)){
      //mantissa is zero: infinity
      if(*s){
        return NEG_INF;
      }
      return POS_INF;
    }
    return NAN;
  }
}

fpclass_t decomposeLongDouble(int *s, int32_t *E, uint64_t *m, long double x){
  longdblcst_t t;
  t.f = x;
  *s = (t.i>>79) & 1;
  *E = (t.i >> 64) & ((((__uint128_t)1)<<15)-1);
  *m = t.i & ((((__uint128_t)1) << 64)- 1);

  if(*E != 32767){
    //x is a NUMBER
    if(*E != 0){
      //x is normalized
      *E = *E - 16383 - 63;
    }
    else{
      //binary exponent is zero: x is denormalized or zero
      if(m != 0){
        *E = 1 - 16383 - 63;
      }
      //else *E == 0 and m == 0: zero
    }
    return NUMBER;
  }
  else{
    //exponent == 32767: all ones
    if((*m >> 62) == 0){
      //case bits 63 and 62 are 00
      if(!(*m)){
        //case all other mantissa bits are also 0, means infinity
        if(*s){
          return NEG_INF;
        }
        return POS_INF;
      }
      return NAN;//case not all other mantissa bits are 0
    }

    else if((*m >> 62) == 1){
      //case bits 63 and 62 are 01
      return NAN;
    }

    else if((*m >> 62) == 2){
      //case bits 63 and 62 are 10
      if((*m << 2) == 0){
        //case all other mantissa bits are also 0, means infinity
        if(*s){
          return NEG_INF;
        }
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




//functions to compute the decimal exponent and mantissa
//given the binary exponent and mantissa

int64_t decimalExponent(int32_t E){
  // 13 bits max
  int64_t constantLog = 330985980541; // = (2^40)*log10(2)
  return (((int64_t)E * constantLog) >> 40)+1;
}

int32_t Delta(int64_t F){
  // = floor(log2(5^(-F))), 14 bits max
  uint64_t constantLog = 2552986939188;// floor(log2(5)*2^40)
  return (int32_t)(((__int128_t)(-F)*constantLog) >> 40);
}

int64_t Sigma(int32_t E, int64_t F, int32_t delta){
  return (E-F-126+delta);
}

int Tau(int64_t F, int Fh, int Fl){
  return 2-Delta(F)+Delta(256*Fh)+Delta(Fl);
}

void optimalT(int64_t F, uint64_t *tH, uint64_t *tL){
  int Fh, Fl, tau;
  uint64_t t1H, t1L, t2H, t2L;
  __uint128_t TH, TM1, TM2, TL, cin, th, tm1, tm2, tl;

  Fh = F>>8;
  Fl = F-(Fh<<8);

  t1H = t1High[Fh+Fhbias];
  t1L = t1Low[Fh+Fhbias];
  t2H = t2High[Fl];
  t2L = t2Low[Fl];
  TL = (__uint128_t)t2L*t1L;
  TH = (__uint128_t)t2H*t1H;
  TM1 = (__uint128_t)t1H*t2L;
  TM2 = (__uint128_t)t1L*t2H;
  // t*2^(128) = TH*2^(128)+(TM1+TM2)*2^(64)+TL

  // t*2^(128) = th*2^(192)+tm1*2^(128)+tm2*2^(64)+tl

  tl = (((__uint128_t)1<<64)-1)&TL;
  tm1 = (TM1&(((__uint128_t)1<<64)-1))+(TM2&(((__uint128_t)1<<64)-1))+(TL>>64);
  cin = tm1>>64;
  tm1 -= cin<<64;
  tm2 = (TH&(((__uint128_t)1<<64)-1))+((TM1>>64)+(TM2>>64))+cin;
  cin = tm2>>64;
  tm2 -= cin<<64;
  th = (TH>>64)+cin;

  tau = Tau(F,Fh,Fl);
  th <<= tau;
  tm2 <<= tau;
  cin = tm2>>64;
  tm2 -= cin<<64;
  th += cin;

  //t = tHigh*2^(64)+tLow
  *tH = (uint64_t)th;
  *tL = (uint64_t)tm2;
}

void multiplyM(int64_t F, uint64_t m, __uint128_t *nHigh, __uint128_t *nMid, __uint128_t *nLow){
  // returns t*m
  __uint128_t cin;
  uint64_t tH, tL;

  optimalT(F, &tH, &tL);

  *nLow = (__uint128_t)m*tL;
  *nMid = (__uint128_t)m*tH;


  *nHigh = *nMid>>64;

  *nMid &= ((__uint128_t)1<<64)-1;// passing on 64 highest bits to nHigh

  *nMid += *nLow>>64;
  *nLow &= ((__uint128_t)1<<64)-1;// passing on 64 highest bits to nMid

  cin = *nMid>>64;
  *nHigh += cin;
  *nMid -= cin<<64;
}


__uint128_t decimalMantissa(int64_t F, uint64_t m, int32_t E){

  int64_t sigma;
  uint32_t shift;

  sigma = Sigma(E,F,Delta(F));// -130<= sigma<= -127

  shift = (uint32_t)(-sigma);
  __uint128_t binary = (__uint128_t)1 << (shift - 65);
  __uint128_t nHigh, nMid, nLow, cin;

  multiplyM(F, m, &nHigh, &nMid, &nLow);
  nMid += binary;
  cin = nMid>>64;
  nHigh += cin;

  if(shift == 127){
    nMid >>= (shift-64);
    nHigh <<= (128-shift);
    nHigh += nMid;
  }
  else{
    nHigh >>= (shift-128);
  }
  return nHigh;
}

void adjustMantissa(int32_t *E, uint64_t *m){
  //we want 2^63 <= m < 2^64
  if(*m < (((uint64_t)1) << 63)){
    if(*m < (((uint64_t)1) << 32)){
      *m <<= 32;
      *E -= 32;
    }
    if(*m < (((uint64_t)1) << 48)){
      *m <<= 16;
      *E -= 16;
    }
    if(*m < (((uint64_t)1) << 56)){
      *m <<= 8;
      *E -= 8;
    }
    if(*m < (((uint64_t)1) << 60)){
      *m <<= 4;
      *E -= 4;
    }
    if(*m < (((uint64_t)1) << 63)){
      *m <<= 2;
      *E -= 2;
    }
    if(*m < (((uint64_t)1) << 63)){
      *m <<= 1;
      *E -= 1;
    }
  }
}

void conversion(int64_t *F, __uint128_t *n, int32_t E, uint64_t m){
  adjustMantissa(&E, &m);
  *F = decimalExponent(E);
  *n = decimalMantissa(*F, m, E);
}
