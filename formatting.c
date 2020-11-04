#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "conversion.h"
#include "formatting.h"



//string manipulation functions

void str_rev(char *s, size_t sz){
  if(sz <= 1)
    return;
  char *p = &s[sz-1];
  while(s < p){
    *s ^= *p;
    *p ^= *s;
    *s ^= *p;
    s++;
    p--;
  }
}

void PUTS(char *s){
  while(*s){
    putchar(*s++);
  }
}

void string_s(char *s, char *str, unsigned int strIndex, P_mode Padding_F, unsigned int length, unsigned int padding_length){
  int i;
  if(Padding_F != RIGHT_PADDED){
    for(i = 0; i < padding_length; i++){
      str[i+strIndex] = ' ';
    }
    strIndex += i;
  }
  for(i = 0; i < length; i++){
    str[i+strIndex] = s[i];
  }
  strIndex += length;
  if(Padding_F == RIGHT_PADDED){
    for(i = 0; i < padding_length; i++){
      str[i+strIndex] = ' ';
    }
  }
}

//integer type to string conversion functions

int string_d(int64_t a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  uint64_t b;
  int neg = 0;
  if(a < 0){
    b = -a;
    neg = 1;
  }
  else{
    b = a;
  }
  int i = 0;
  while(b != 0){
    s[i++] = '0'+b%10;
    b /= 10;
  }
  if(neg)
    s[i++] = '-';
  s[i] = '\0';
  str_rev(s, i);
  return i;
}


//octal unsigned

int string_u(uint64_t a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  int i = 0;
  while(a != 0){
    s[i++] = '0'+a%10;
    a /= 10;
  }
  s[i] = '\0';
  str_rev(s, i);
  return i;
}


//hexadecimal unsigned

int string_x(uint64_t a, char *s, int mod){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  int tmp = 'a';
  if(mod == 1)
    tmp = 'A';
  int i = 0;
  int h;
  while(a != 0){
    h = a&15u;
    a >>= 4;
    if(h < 10)
      s[i++] = '0'+h;
    else
      s[i++] = tmp+h;
  }
  if(mod == 1)
    s[i++] = 'X';
  else
    s[i++] = 'x';
  s[i++] = '0';
  s[i] = '\0';
  str_rev(s, i);
  return i;
}



// octal unsigned

int string_o(uint64_t a, char *s){
  if(a==0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  int o;
  int i = 0;
  while (a != 0){
    o = a&7u;
    s[i++] = '0'+o;
    a >>= 3;
  }
  s[i++] = '0';
  s[i] = '\0';
  str_rev(s, i);
  return i;
}

//floating point conversion functions

//exponent format
//6 decimals by default, 18 max

int string_e(double f, char *str, unsigned int prec){
  int s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  __uint128_t n; //decimal mantissa
  fpclass_t class;
  class = decomposeDouble(&s, &E, &m, f);
  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_e(str, prec, s, n, F);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

int string_Le(long double f, char *str, unsigned int prec){
  int s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  __uint128_t n; //decimal mantissa
  fpclass_t class;
  class = decomposeLongDouble(&s, &E, &m, f);
  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_e(str, prec, s, n, F);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

//floating point format
//do not use for values greater than +/-10^(-19)

int string_f(double f, char *str, unsigned int prec){
  int s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  __uint128_t n; //decimal mantissa
  fpclass_t class;
  class = decomposeDouble(&s, &E, &m, f);
  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_f(str, prec, s, n, F);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

int string_Lf(long double f, char *str, unsigned int prec){
  int s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  __uint128_t n; //decimal mantissa
  fpclass_t class;
  class = decomposeLongDouble(&s, &E, &m, f);
  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_f(str, prec, s, n, F);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

//general format

int string_g(double f, char *str, unsigned int prec){
  int s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  __uint128_t n; //decimal mantissa
  fpclass_t class;
  class = decomposeDouble(&s, &E, &m, f);
  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_g(str, prec, s, n, F);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

int string_Lg(long double f, char *str, unsigned int prec){
  int s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  __uint128_t n; //decimal mantissa
  fpclass_t class;
  class = decomposeLongDouble(&s, &E, &m, f);
  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_g(str, prec, s, n, F);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}


int fp_special_case(fpclass_t class, char *str){
  int i = 0;
  if(class == POS_INF){
    str[i++] = 'i';
    str[i++] = 'n';
    str[i++] = 'f';
    str[i] = '\0';
  }
  else if(class == NEG_INF){
    str[i++] = '-';
    str[i++] = 'i';
    str[i++] = 'n';
    str[i++] = 'f';
    str[i] = '\0';
  }
  else{
    str[i++] = 'n';
    str[i++] = 'a';
    str[i++] = 'n';
    str[i] = '\0';
  }
  return i;
}

int fp_fmt_e(char *str, unsigned int prec, int s, __uint128_t n, int32_t F){
  if(prec > 18)
    prec = 18;

  //write sign
  int i = 0;
  if(s)
    str[i++] = '-';

  if(n == 0){
    //f == 0.
    str[i++] = '0';
    if(prec){
      str[i++] = '.';
      for(int j = 0; j < prec; j++){
        str[i++] = '0';
      }
    }
    str[i++] = 'e';
    str[i++] = '+';
    str[i++] = '0';
    str[i++] = '0';
    str[i] = '\0';
    return i;
  }

  int l = 20;
  char d[l]; //to write decimal mantissa
  char e[l]; //to write decimal exponent
  //n has 19 digits
  string_u(n,d);
  //we want 1 before the decimal point, 18 after
  //so we increase the decimal exponent by 18
  F += 18;
  if(F > -10 && F < 10){
    //we want e to be written with at least 2 digits
    //example: e-05, e+02, e+00
    int k = 0;
    if(F < 0){
      e[k++] = '-';
      F = -F;
    }
    e[k++] = '0';
    e[k++] = '0'+F;
    e[k] = '\0';
  }
  else{
    string_d(F,e);
  }

  //write decimal mantissa
  str[i++] = d[0];
  if(prec){
    str[i++] = '.';
    for(int j = 1; j <= prec; j++){
      str[i++] = d[j];
    }
  }

  //write decimal exponent
  str[i++] = 'e';
  int j = 0;
  if(e[j] == '-')
    str[i++] = e[j++];
  else
    str[i++] = '+';
  while(e[j]){
    str[i++] = e[j++];
  }
  str[i] = '\0';
  return i;
}

int fp_fmt_f(char *str, unsigned int prec, int s, __uint128_t n, int32_t F){
  if(prec > 18)
    prec = 18;

  //write sign
  int i = 0;
  if(s)
    str[i++] = '-';

  if(n == 0){
    //f == 0.
    str[i++] = '0';
    if(prec){
      str[i++] = '.';
      for(int j = 0; j < prec; j++){
        str[i++] = '0';
      }
    }
    str[i] = '\0';
    return i;
  }

  int l = 20;
  char d[l]; //to write decimal mantissa
  char e[l]; //to write decimal exponent
  //n has 19 digits
  string_d(F,e);
  string_u(n,d);

  //write decimal mantissa
  int digits = 19+F; //digits before decimal point
  int j = 0; //used to index decimal mantissa
  if(digits >= 19){
    //write whole mantissa
    //then add zeros until decimal point
    //add prec zeros after decimal point
    for(j = 0; j < 19; j++){
      str[i++] = d[j];
    }
    while(j < digits){
      str[i++] = '0';
      j++;
    }
    if(prec){
      str[i++] = '.';
      for(int k = 0; k < prec; k++){
        str[i++] = '0';
      }
    }
  }
  else if(digits > 0){
    //write part of the decimal mantissa before decimal point
    //then maybe write the rest afterwards, depending on prec
    //complete with zeros if not enough digits for prec
    for(j = 0; j < digits; j++){
      str[i++] = d[j];
    }
    if(prec){
      str[i++] = '.';
      int decimals = 19-j; //unconsumed digits on decimal mantissa
      //that we haven't consumed yet
      for(int k = 0; k < prec; k++){
        if(k < decimals)
          str[i++] = d[j++];
        else
          str[i++] = '0';
      }
    }
  }
  else{
    //0.0...0dddd
    //the decimal mantissa starts
    //after the decimal point
    str[i++] = '0';
    if(prec){
      str[i++] = '.';
      digits = -digits;
      for(int k = 0, j = 0; k < prec; k++){
        if(k < digits)
          str[i++] = '0';
        else
          str[i++] = d[j++];
      }
    }
  }
  str[i] = '\0';
  return i;
}

int fp_fmt_g(char *str, unsigned int prec, int s, __uint128_t n, int32_t F){
  if(prec > 18)
    prec = 18;
  if(prec == 0)
    prec = 1;

  //write sign
  int i = 0;
  if(s)
    str[i++] = '-';

  if(n == 0){
    //f == 0.
    str[i++] = '0';
    str[i] = '\0';
    return i;
  }
  int l = 20;
  char d[l]; //to write decimal mantissa
  char e[l]; //to write decimal exponent
  //n has 19 digits
  string_u(n,d);

  //if written in 'e'-format, the exponent would be F+18
  if(F+18 >= (int)prec || F+18 < -4){
    //printf("%d\n", F);
    //'e'-format conversion
    F += 18;
    string_d(F,e);
    str[i++] = d[0];
    if(prec > 1){
      str[i++] = '.';
      for(int j = 1; j < prec; j++){
        str[i++] = d[j];
      }
      int k = 0;
      str[i++] = 'e';
      if(e[k] == '-')
        str[i++] = e[k++];
      else
        str[i++] = '+';
      if(e[k] == '0'){
        //exponent is zero, has to be written as 00
        str[i++] = '0';
        str[i++] = '0';
      }
      else{
        if(F > -10 && F < 10)
          str[i++] = '0';
        while(e[k]){
          str[i++] = e[k++];
        }
      }
    }
    str[i] = '\0';
    return i;
  }

  int digits = 19+F; //digits before the decimal point
  int j = 0; //where we are on our decimal mantissa
  if(digits <= 0){
    //0.0... ddd
    str[i++] = '0';
  }
  else{
    for(int k = 0; k < digits; k++){
      str[i++] = d[j++];
    }
  }
  if(j < prec){
    str[i++] = '.';
    while(digits < 0){
      str[i++] = '0';
      digits++;
    }
    while(j < prec){
      str[i++] = d[j++];
    }
    //remove trailing zeros
    while(str[i-1] == '0'){
      i--;
    }
    if(str[i-1] == '.')
      i--;
  }
  str[i] = '\0';
  return i;
}
