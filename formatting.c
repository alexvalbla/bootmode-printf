#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "conversion.h"
#include "formatting.h"



//string manipulation functions

void str_rev(char *s, size_t size){
  if(size <= 1)
    return;
  char *p = &s[size-1];
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
    putchar(*(s++));
  }
}


//integer type to string conversion functions

int convert_d(va_list ap, char mods[2], char *str, uint8_t flags){
  int64_t a;
  if(mods[0] == 'l' && mods[1] == '\0'){
    a = va_arg(ap, long);
  }
  else if(mods[0] == 'l' && mods[1] == 'l'){
    a = va_arg(ap, long long);
  }
  else if(mods[0] == 'h' && mods[1] == '\0'){
    a = va_arg(ap, int);
  }
  else if(mods[0] == 'h' && mods[1] == 'h'){
    a = va_arg(ap, int);
  }
  else{
    a = va_arg(ap, int);
  }
  return int_fmt_d(a, str, flags);
}

int convert_u(va_list ap, char mods[2], char *str, uint8_t flags){
  uint64_t a;
  if(mods[0] == 'l' && mods[1] == '\0'){
    a = va_arg(ap, unsigned long);
  }
  else if(mods[0] == 'l' && mods[1] == 'l'){
    a = va_arg(ap, unsigned long long);
  }
  else if(mods[0] == 'h' && mods[1] == '\0'){
    a = va_arg(ap, unsigned int);
  }
  else if(mods[0] == 'h' && mods[1] == 'h'){
    a = va_arg(ap, unsigned int);
  }
  else{
    a = va_arg(ap, unsigned int);
  }
  return int_fmt_u(a, str, flags);
}

int convert_x(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
  uint64_t a;
  if(mods[0] == 'l' && mods[1] == '\0'){
    a = va_arg(ap, unsigned long);
  }
  else if(mods[0] == 'l' && mods[1] == 'l'){
    a = va_arg(ap, unsigned long long);
  }
  else if(mods[0] == 'h' && mods[1] == '\0'){
    a = va_arg(ap, unsigned int);
  }
  else if(mods[0] == 'h' && mods[1] == 'h'){
    a = va_arg(ap, unsigned int);
  }
  else{
    a = va_arg(ap, unsigned int);
  }
  return int_fmt_x(a, str, prec, flags);
}

int convert_o(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
  uint64_t a;
  if(mods[0] == 'l' && mods[1] == '\0'){
    a = va_arg(ap, unsigned long);
  }
  else if(mods[0] == 'l' && mods[1] == 'l'){
    a = va_arg(ap, unsigned long long);
  }
  else if(mods[0] == 'h' && mods[1] == '\0'){
    a = va_arg(ap, unsigned int);
  }
  else if(mods[0] == 'h' && mods[1] == 'h'){
    a = va_arg(ap, unsigned int);
  }
  else{
    a = va_arg(ap, unsigned int);
  }
  return int_fmt_o(a, str, prec, flags);
}

void convert_n(va_list ap, char mods[2], ssize_t total){
  if(mods[0] == '\0'){
    int *n = va_arg(ap, int*);
    *n = total;
  }
  else if(mods[0] == 'l' && mods[1] == '\0'){
    long *n = va_arg(ap, long*);
    *n = total;
  }
  else if(mods[0] == 'l' && mods[1] == 'l'){
    long long *n = va_arg(ap, long long*);
    *n = total;
  }
  else if(mods[0] == 'h' && mods[1] == '\0'){
    short *n = (short*)va_arg(ap, int*);
    *n = total;
  }
  else if(mods[0] == 'h' && mods[1] == 'h'){
    char *n = (char*)va_arg(ap, int*);
    *n = total;
  }
}




int int_fmt_d(int64_t a, char *str, uint8_t flags){
  if(a == 0){
    str[0] = '0';
    str[1] = '\0';
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
    str[i++] = '0'+b%10;
    b /= 10;
  }
  if(neg)
    str[i++] = '-';
  str[i] = '\0';
  str_rev(str, i);
  return i;
}

int int_fmt_u(uint64_t a, char *str, uint8_t flags){
  if(a == 0){
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }
  int i = 0;
  while(a != 0){
    str[i++] = '0'+a%10;
    a /= 10;
  }
  str[i] = '\0';
  str_rev(str, i);
  return i;
}

int int_fmt_x(uint64_t a, char *str, uint16_t prec, uint8_t flags){
  //if no precision specified, precision is 1
  if(!(flags&FLAG_PREC))
    prec = 1;

  if(a == 0){
    int i = 0;
    while(i < prec){
      str[i++] = '0';
    }
    str[i] = '\0';
    return i;
  }
  char tmp_c = 'a';
  char tmp_x = 'x';
  if(flags&FLAG_UCAS){
    tmp_c = 'A';
    tmp_x = 'X';
  }
  int i = 0;
  int h;
  while(a != 0){
    h = a&15u;
    a >>= 4;
    if(h < 10)
      str[i++] = '0'+h;
    else
      str[i++] = tmp_c+h;
  }
  while(i < prec){
    str[i++] = '0';
  }
  if(flags&FLAG_ALTF){
    str[i++] = tmp_x;
    str[i++] = '0';
  }
  str[i] = '\0';
  str_rev(str, i);
  return i;
}

int int_fmt_o(uint64_t a, char *str, uint16_t prec, uint8_t flags){
  //if no precision specified, precision is 1
  if(!(flags&FLAG_PREC))
    prec = 1;

  if(a == 0){
    int i = 0;
    while(i < prec){
      str[i++] = '0';
    }
    if(flags&FLAG_ALTF && i == 0)
      str[i++] = '0';
    str[i] = '\0';
    return i;
  }
  int o;
  int i = 0;
  while (a != 0){
    o = a&7u;
    str[i++] = '0'+o;
    a >>= 3;
  }
  if(flags&FLAG_ALTF)
    str[i++] = '0';
  str[i] = '\0';
  str_rev(str, i);
  return i;
}


//floating point conversion functions

int convert_e(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
  char s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  uint64_t n; //decimal mantissa
  fpclass_t class;
  if(mods[0] == 'L' && mods[1] == '\0')
    class = decomposeLongDouble(&s, &E, &m, va_arg(ap, long double));
  else
    class = decomposeDouble(&s, &E, &m, va_arg(ap, double));

  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_e(str, s, n, F, prec, flags);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

int convert_f(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
  char s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  uint64_t n; //decimal mantissa
  fpclass_t class;
  if(mods[0] == 'L' && mods[1] == '\0')
    class = decomposeLongDouble(&s, &E, &m, va_arg(ap, long double));
  else
    class = decomposeDouble(&s, &E, &m, va_arg(ap, double));

  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_f(str, s, n, F, prec, flags);
  }
  //otherwise: +inf, -inf or nan
  return fp_special_case(class, str);
}

int convert_g(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
  char s; //sign
  int32_t E; //binary exponent
  uint64_t m; //binary mantissa
  int32_t F; //decimal exponent
  uint64_t n; //decimal mantissa
  fpclass_t class;
  if(mods[0] == 'L' && mods[1] == '\0')
    class = decomposeLongDouble(&s, &E, &m, va_arg(ap, long double));
  else
    class = decomposeDouble(&s, &E, &m, va_arg(ap, double));

  if(class == NUMBER){
    if(m == 0)
      n = 0;
    decimalConversion(&F, &n, E, m);
    return fp_fmt_g(str, s, n, F, prec, flags);
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

int fp_fmt_e(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags){
  //adjust precision
  if(flags&FLAG_PREC){
    if(prec > 18)
      prec = 18;
  }
  else{
    //precision missing, taken as 6
    prec = 6;
  }

  //write sign
  int i = 0;
  if(s)
    str[i++] = '-';

  if(n == 0){
    //f == 0.
    str[i++] = '0';
    if(prec || (flags&FLAG_ALTF)){
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
  int_fmt_u(n,d,0);
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
    int_fmt_d(F,e,0);
  }

  //write decimal mantissa
  str[i++] = d[0];
  if(prec || (flags&FLAG_ALTF)){
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

int fp_fmt_f(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags){
  //adjust precision
  if(flags&FLAG_PREC){
    if(prec > 19)
      prec = 19;
  }
  else{
    //precision missing, taken as 6
    prec = 6;
  }
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
  //n has 19 digits
  int_fmt_u(n,d,0);

  //write decimal mantissa
  int digits = 19+F; //digits before decimal point
  int j = 0; //used to index decimal mantissa
  if(digits >= 19){
    //write the whole mantissa
    //then add zeros until decimal point
    //add prec zeros after decimal point
    for(j = 0; j < 19; j++){
      str[i++] = d[j];
    }
    while(j < digits){
      str[i++] = '0';
      j++;
    }
    if(prec || (flags&FLAG_ALTF)){
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
    if(prec || (flags&FLAG_ALTF)){
      str[i++] = '.';
      int decimals = 19-j; //digits on decimal mantissa...
      //...that we haven't consumed yet
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
    if(prec || (flags&FLAG_ALTF)){
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

int fp_fmt_g(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags){
  //adjust precision
  if(flags&FLAG_PREC){
    if(prec > 19)
      prec = 19;
    else if(prec == 0)
      prec = 1;
  }
  else{
    //precision missing, taken as 6
    prec = 6;
  }

  //write sign
  int i = 0;
  if(s)
    str[i++] = '-';

  if(n == 0){
    //f == 0.
    str[i++] = '0';
    if(flags&FLAG_ALTF){
      str[i++] = '.';
      for(int k = 1; k < prec; k++){
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
  int_fmt_u(n,d,0);

  //if written in 'e'-format, the exponent would be F+18
  if(F+18 >= (int)prec || F+18 < -4){
    //'e'-format conversion
    F += 18;
    int_fmt_d(F,e,0);
    str[i++] = d[0];
    if(prec > 1 || (flags&FLAG_ALTF)){
      str[i++] = '.';
      for(int j = 1; j < prec; j++){
        str[i++] = d[j];
      }
      if(!(flags&FLAG_ALTF)){
        //remove trailing zeros
        //and decimal point if no decimals follow it
        while(str[i-1] == '0'){
          i--;
        }
        if(str[i-1] == '.')
          i--;
      }
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
    str[i] = '\0';
    return i;
  }

  //'f'-format conversion
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
  if(j < prec || (flags&FLAG_ALTF)){
    str[i++] = '.';
    while(digits < 0){
      str[i++] = '0';
      digits++;
    }
    while(j < prec){
      str[i++] = d[j++];
    }
    //remove trailing zeros
    if(!(flags&FLAG_ALTF)){
      //remove trailing zeros
      //and decimal point if no decimals follow it
      while(str[i-1] == '0'){
        i--;
      }
      if(str[i-1] == '.')
        i--;
    }
  }
  str[i] = '\0';
  return i;
}
