#include <stdint.h>
#include <stdio.h>
#include "conversion.h"
#include "tableT1.h"
#include "tableT2.h"


int64_t decimal_exponent(int32_t E){
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

void conversion(int64_t *F, __uint128_t *n, int32_t E, uint64_t m){
  *F = decimal_exponent(E);
  *n = decimalMantissa(*F, m, E);

}
