#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decomposition.h"
#include "conversion.h"
#include "auxFunctions.h"



//string manipulation functions

void reverse_string(char *s, unsigned int size){
  char tmp;
  int i;
  for(i = 0; i < size/2; i++){
    tmp = s[i];
    s[i] = s[size-i-1];
    s[size-i-1] = tmp;
  }
}

void print_string(char *s){
  while(*s != '\0'){
    putchar(*s);
    s++;
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

int string_d(int a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int b;
  unsigned int i=0, neg=0;
  if(a < 0){
    b = -a;
    neg = 1;
  }
  else b = a;
  while(b != 0){
    s[i] = '0'+(b%10);
    b /= 10;
    i++;
  }
  if(neg){
    s[i] = '-';
    i++;
  }
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_ld(long int a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int i=0, neg=0;
  unsigned long int b;
  if(a < 0){
    b = -a;
    neg = 1;
  }
  else b = a;
  while(b != 0){
    s[i] = '0'+(b%10);
    b /= 10;
    i++;
  }
  if(neg){
    s[i] = '-';
    i++;
  }
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_lld(long long int a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int i=0, neg=0;
  unsigned long long int b;
  if(a < 0){
    b = -a;
    neg = 1;
  }
  else b = a;
  while(b != 0){
    s[i] = '0'+(b%10);
    b /= 10;
    i++;
  }
  if(neg){
    s[i] = '-';
    i++;
  }
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

//unsigned

int string_u(unsigned int a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int i=0;
  while(a != 0){
    s[i] = '0'+(a%10);
    a /= 10;
    i++;
  }
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_lu(long unsigned int a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  int i=0;
  while(a!=0){
    s[i] = '0'+(a%10);
    a /= 10;
    i++;
  }
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_llu(long long unsigned int a, char *s){
  if(a == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  int i=0;
  while(a!=0){
    s[i] = '0'+(a%10);
    a /= 10;
    i++;
  }
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

//hexadecimal unsigned

int string_x(unsigned int n, char *s, int mod){
  if(n == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int i = 0;
  unsigned int c;
  unsigned int tmp;
  if(mod == 1) tmp = 'A';
  else tmp = 'a';
  while(n != 0){
    c = n&((1u<<4)-1);
    n >>=4;
    if(c<10){
      s[i] = '0'+c;
    }
    else{
      s[i] = tmp+c-10;
    }
    i++;
  }
  if(mod == 1) s[i] = 'X';
  else s[i] = 'x';
  i++;
  s[i] = '0';
  i++;
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_lx(long unsigned int n, char *s, int mod){
  if(n == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int i = 0;
  unsigned int c;
  unsigned int tmp;
  if(mod == 1) tmp = 'A';
  else tmp = 'a';
  while(n != 0){
    c = n&((1u<<4)-1);
    n >>=4;
    if(c<10){
      s[i] = '0'+c;
    }
    else{
      s[i] = tmp+c-10;
    }
    i++;
  }
  if(mod == 1) s[i] = 'X';
  else s[i] = 'x';
  i++;
  s[i] = '0';
  i++;
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_llx(long long unsigned int n, char *s, int mod){
  if(n == 0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int i = 0;
  unsigned int c;
  unsigned int tmp;
  if(mod == 1) tmp = 'A';
  else tmp = 'a';
  while(n != 0){
    c = n&((1u<<4)-1);
    n >>=4;
    if(c<10){
      s[i] = '0'+c;
    }
    else{
      s[i] = tmp+c-10;
    }
    i++;
  }
  if(mod == 1) s[i] = 'X';
  else s[i] = 'x';
  i++;
  s[i] = '0';
  i++;
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

// octal unsigned

int string_o(unsigned int n, char *s){
  if(n==0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int oct;
  unsigned int i = 0;
  while (n != 0){
    oct = n&(8u-1);
    s[i] = '0'+oct;
    i++;
    n >>= 3;
  }
  s[i] = '0';
  i++;
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_lo(unsigned long int n, char *s){
  if(n==0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int oct;
  unsigned int i = 0;
  while (n != 0){
    oct = n&(8u-1);
    s[i] = '0'+oct;
    i++;
    n >>= 3;
  }
  s[i] = '0';
  i++;
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

int string_llo(unsigned long long int n, char *s){
  if(n==0){
    s[0] = '0';
    s[1] = '\0';
    return 1;
  }
  unsigned int oct;
  unsigned int i=0;
  while (n!=0){
    oct = n&(8u-1);
    s[i] = '0'+oct;
    i++;
    n >>= 3;
  }
  s[i] = '0';
  i++;
  s[i] = '\0';
  reverse_string(s, i);
  return i;
}

//floating point conversion functions

//exponent format
//6 decimals by default, 18 max

int string_e(double f, char *s, unsigned int decimal){
  int sign, E, i, j=0, numN;
  int64_t F;
  uint64_t m;
  __uint128_t n;
  fpclass_t class;
  class = decomposeDouble(&sign, &E, &m, f);
  if(class == NUMBER){

    if(E == 0 && m == 0){
      s[0] = '0';
      s[1] = '\0';
      return 1;
    }
    conversion(&F, &n, E, m);
    char N[25];//mantissa
    char e[25];//exponent
    numN = string_lu(n, N);
    F += numN-1;//result of point replacement
    string_d(F,e);

    if(sign){
      s[j] = '-';
      j++;
    }

    s[j] = N[0];
    j++;
    s[j] = '.';
    j++;
    for(i=1; i <= decimal; i++){
      if(N[i] == '\0'){
        break;
      }
      s[j] = N[i];
      j++;
    }
    while(i <= decimal){
      s[j] = '0';
      j++;
      i++;
    }

    s[j] = 'e';
    j++;
    i=0;

    if(e[0] != '-'){
      s[j] = '+';
      j++;
      if(e[1] == '\0'){
        s[j] = '0';
        j++;
      }
    }
    else{
      s[j] = '-';
      i++;
      j++;
      if(e[2] == '\0'){
        s[j] = '0';
        j++;
      }
    }

    while(e[i]){
      s[j] = e[i];
      i++;
      j++;
    }
    s[j] = '\0';
    return j;
  }

  else if(class == NAN){
    s[0] = 'n';
    s[1] = 'a';
    s[2] = 'n';
    s[3] = '\0';
    return 3;
  }
  else if(class == POS_INF){
    s[0] = 'i';
    s[1] = 'n';
    s[2] = 'f';
    s[3] = '\0';
    return 3;
  }
  else if(class == NEG_INF){
    s[0] = '-';
    s[1] = 'i';
    s[2] = 'n';
    s[3] = 'f';
    s[4] = '\0';
    return 4;
  }
  return 0;
}

int string_Le(long double f, char *s, unsigned int decimal){
  //6 decimals by default , 18 max CONTAINS BUGS!
  int sign, E,i, j=0, numN;
  int64_t F;
  uint64_t m;
  __uint128_t n;
  fpclass_t class;
  class = decomposeLongDouble(&sign, &E, &m, f);
  if(class == NUMBER){

    if(E == 0 && m == 0){
      s[0] = '0';
      s[1] = '\0';
      return 1;
    }
    conversion(&F, &n, E, m);
    char N[25];//mantissa
    char e[25];//exponent
    numN = string_lu(n,N);
    F += numN-1;// result of point replacement
    string_d(F,e);

    if(sign){
      s[j] = '-';
      j++;
    }

    s[j] = N[0];
    j++;
    s[j] = '.';
    j++;
    for(i=1; i<=decimal; i++){
      if(N[i] == '\0'){
        break;
      }
      s[j] = N[i];
      j++;
    }
    while(i <= decimal){
      s[j] = '0';
      j++;
      i++;
    }

    s[j] = 'e';
    j++;
    i=0;
    if(e[0] != '-'){
      s[j] = '+';
      j++;
      if(e[1] == '\0'){
        s[j] = '0';
        j++;
      }
    }
    else{
      s[j] = '-';
      i++;
      j++;
      if(e[2] == '\0'){
        s[j] = '0';
        j++;
      }
    }

    while(e[i]){
      s[j] = e[i];
      i++;
      j++;
    }
    s[j] = '\0';
    return j;
  }

  else if(class == NAN){
    s[0] = 'n';
    s[1] = 'a';
    s[2] = 'n';
    s[3] = '\0';
    return 3;
  }
  else if(class == POS_INF){
    s[0] = 'i';
    s[1] = 'n';
    s[2] = 'f';
    s[3] = '\0';
    return 3;
  }
  else if(class == NEG_INF){
    s[0] = '-';
    s[1] = 'i';
    s[2] = 'n';
    s[3] = 'f';
    s[4] = '\0';
    return 4;
  }
  return 0;
}

//floating point format
//do not use for values greater than +/-10^(-19)

int string_f(double f, char *s, unsigned int decimal){
  int sign, E, i, j ,numN, pointPos;
  int64_t F;
  uint64_t m;
  __uint128_t n;
  fpclass_t class;
  class = decomposeDouble(&sign, &E, &m, f);

  if(class == NUMBER){

    if((E==0 && m==0) || E <= -132){
      // number is zero
      s[0] = '0';
      s[1] = '.';
      for(i=0; i<decimal; i++){
        s[i+2] = '0';
      }
      s[i+2] = '\0';
      return 2+i;
    }
   
    conversion(&F, &n, E, m);
    char mantissa[40];
    numN = string_lu(n, mantissa);
    i = 0;
    if(sign){
      s[i] = '-';
      i++;
    }

    //three cases
    pointPos = F+numN;
    if(pointPos<=0){
      // case 0.0...ddd
      s[i] = '0';
      i++;
      s[i] = '.';
      i++;
      while(pointPos!=0){
        s[i] = '0';
        i++;
        pointPos++;
	if(decimal>0){
	decimal--;
	}
      }
      j = 0;
      while(j<decimal){
        if(j>=numN){
          s[i] = '0';
        }
        else{
          s[i] = mantissa[j];
        }
        i++;
        j++;
      }
    }
    else if(pointPos>0&&pointPos<=20){
      pointPos+=sign;
      // case ddd.ddd
      j = 0;
      while(j<decimal+pointPos){
        if(i == pointPos){
          s[i] = '.';
          i++;
        }
        if(j<numN){
          s[i] = mantissa[j];
        }
        else{
          s[i] = '0';
        }
        i++;
        j++;
      }
    }

   
    else{
      //case number too large
      char err [] = "Error: the argument is too big to be formatted as %f. Use %e or %g instead\n";
      print_string(err);
      s[0] = '\0';
    }
    s[i] = '\0';
    return i;
  }
  else if(class == NAN){
    s[0] = 'n';
    s[1] = 'a';
    s[2] = 'n';
    s[3] = '\0';
    return 3;
  }
  else if(class == POS_INF){
    s[0] = 'i';
    s[1] = 'n';
    s[2] = 'f';
    s[3] = '\0';
    return 3;
  }
  else if(class == NEG_INF){
    s[0] = '-';
    s[1] = 'i';
    s[2] = 'n';
    s[3] = 'f';
    s[4] = '\0';
    return 4;
  }
  return 0;
}

int string_Lf(long double f, char *s, unsigned int decimal){
  int sign, E, i, j ,numN, pointPos;
  int64_t F;
  uint64_t m;
  __uint128_t n;

  fpclass_t class;
  class = decomposeLongDouble(&sign, &E, &m, f);

  if(class == NUMBER){

    if(E==0 && m == 0){
      // number is zero 
      s[0] = '0';
      s[1] = '.';
      for(i=0; i<decimal; i++){
        s[i+2] = '0';
      }
      s[i+2] = '\0';
      return 2+i;
    }
   
    conversion(&F, &n, E, m);
    char mantissa[40];
    numN = string_lu(n, mantissa);
    i = 0;
    if(sign){
      s[i] = '-';
      i++;
    }

    //three cases
    pointPos = F+numN;
    if(pointPos <= 0){
      // case 0.0...ddd
      s[i] = '0';
      i++;
      s[i] = '.';
      i++;
      while(pointPos!=0){
        s[i] = '0';
        i++;
        pointPos++;
		if(decimal>0){
	decimal--;
	}
      }

      j = 0;
      while(j<decimal){
        if(j>=numN){
          s[i] = '0';
        }
        else{
          s[i] = mantissa[j];
        }
        i++;
        j++;
      }

    }
    else if(pointPos>0 &&pointPos<=20){
      // case ddd.ddd
      pointPos+=sign;
      j = 0;
      while(j<decimal+pointPos){
        if(i == pointPos){
          s[i] = '.';
          i++;
        }
        if(j<numN){
          s[i] = mantissa[j];
        }
        else{
          s[i] = '0';
        }
        i++;
        j++;
      }
    }

   
    else{
      //case number too large
      char err [] = "Error: the argument is too big to be formatted as %f. Use %e or %g instead\n";
      print_string(err);
      s[0] = '\0';
    }
    s[i] = '\0';
    return i;
  }
  else if(class == NAN){
    s[0] = 'n';
    s[1] = 'a';
    s[2] = 'n';
    s[3] = '\0';
    return 3;
  }
  else if(class == POS_INF){
    s[0] = 'i';
    s[1] = 'n';
    s[2] = 'f';
    s[3] = '\0';
    return 3;
  }
  else if(class == NEG_INF){
    s[0] = '-';
    s[1] = 'i';
    s[2] = 'n';
    s[3] = 'f';
    s[4] = '\0';
    return 4;
  }
  return 0;
}

//general format

int string_g(double f, char *s, unsigned int decimal){
  int sign, E, i, j ,numN, pointPos;
  int64_t F;
  uint64_t m;
  __uint128_t n;
  fpclass_t class;
  class = decomposeDouble(&sign, &E, &m, f);

  if(class == NUMBER){

    if(E == 0 && m == 0){
      s[0] = '0';
      s[1] = '\0';
      return 1;
    }
    conversion(&F, &n, E, m);
    char mantissa[40];
    numN = string_lu(n, mantissa);
    i = 0;
    if(sign){
      s[i] = '-';
      i++;
    }
    pointPos = F+numN;
    if(pointPos>-5 && pointPos<decimal){
      // %f format will be used
      
      if(pointPos<=0){
        // case 0.0...ddd
        s[i] = '0';
        i++;
        s[i] = '.';
        i++;
        while(pointPos!=0){
          s[i] = '0';
          i++;
          pointPos++;
	  if(decimal>0){
	    decimal--;
	  }
        }
        j = 0;
        while(j<decimal){
          if(j>=numN){
            s[i] = '0';
          }
          else{
            s[i] = mantissa[j];
          }
          i++;
          j++;
        }
      }

      else if(pointPos>0 && pointPos<=20){
        // case ddd.ddd
        pointPos+=sign;
        j = 0;
        while(j<decimal+pointPos){
          if(i == pointPos){
            s[i] = '.';
            i++;
          }
          if(j<numN){
            s[i] = mantissa[j];
          }
          else{
            s[i] = '0';
          }
          i++;
          j++;
        }
      }

    
      s[i] ='\0';
      
      return i;
    }
    else{
      // %e format will be used
      F += numN-1;
      char exponent[20];
      string_ld(F, exponent);
      j=0;
      if(sign){
        s[j] = '-';
        j++;
      }
      s[j] = mantissa[0];
      j++;
      s[j] = '.';
      j++;
      for(i=1; i<=decimal; i++){
        if(mantissa[i] == '\0'){
          s[j] = '0';
        }
        else{
          s[j] = mantissa[i];
        }
        j++;
      }
      s[j] = 'e';
      j++;
      i=0;
      while(exponent[i]){
        s[j] = exponent[i];
        i++;
        j++;
      }
      s[j] = '\0';
      
      return j;
    }
  }
  else if(class == NAN){
    s[0] = 'n';
    s[1] = 'a';
    s[2] = 'n';
    s[3] = '\0';
    return 3;
  }
  else if(class == POS_INF){
    s[0] = 'i';
    s[1] = 'n';
    s[2] = 'f';
    s[3] = '\0';
    return 3;
  }
  else if(class == NEG_INF){
    s[0] = '-';
    s[1] = 'i';
    s[2] = 'n';
    s[3] = 'f';
    s[4] = '\0';
    return 4;
  }
  return 0;
}

int string_Lg(long double f, char *s, unsigned int decimal){
  int sign, E, i, j ,numN, pointPos;
  int64_t F;
  uint64_t m;
  __uint128_t n;
  fpclass_t class;
  class = decomposeLongDouble(&sign, &E, &m, f);

  if(class == NUMBER){

    if(E == 0 && m == 0){
      s[0] = '0';
      s[1] = '\0';
      return 1;
    }
    conversion(&F, &n, E, m);
    char mantissa[40];
    numN = string_lu(n, mantissa);
    i = 0;
    if(sign){
      s[i] = '-';
      i++;
    }
    pointPos = F+numN;
    if(pointPos>-5 && pointPos<decimal){
      // %f format will be used
      if(pointPos<=0){
        // case 0.0...ddd
        s[i] = '0';
        i++;
        s[i] = '.';
        i++;
        while(pointPos!=0){
          s[i] = '0';
          i++;
          pointPos++;
	  if(decimal>0){
	    decimal--;
	  }
        }
        j = 0;
        while(j<decimal){
          if(j>=numN){
            s[i] = '0';
          }
          else{
            s[i] = mantissa[j];
          }
          i++;
          j++;
        }
      }
      else if(pointPos>0 && pointPos<=20){
        // case ddd.ddd
        pointPos+=sign;
        j = 0;
        while(j<decimal+pointPos){
          if(i == pointPos){
            s[i] = '.';
            i++;
          }
          if(j<numN){
            s[i] = mantissa[j];
          }
          else{
            s[i] = '0';
          }
          i++;
          j++;
        }
      }

      s[i] = '\0';
      return i;
    }
    else{
      // %e format will be used
      F += numN-1;
      char exponent[20];
      string_ld(F, exponent);
      j = 0;
      if(sign){
        s[j] = '-';
        j++;
      }
      s[j] = mantissa[0];
      j++;
      s[j] = '.';
      j++;
      for(i=1; i<=decimal; i++){
        if(mantissa[i] == '\0'){
          s[j] = '0';
        }
        else{
          s[j] = mantissa[i];
        }
        j++;
      }
      s[j] = 'e';
      j++;
      i=0;
      while(exponent[i]){
        s[j] = exponent[i];
        i++;
        j++;
      }
      s[j] = '\0';
      return j;
    }
  }
  else if(class == NAN){
    s[0] = 'n';
    s[1] = 'a';
    s[2] = 'n';
    s[3] = '\0';
    return 3;
  }
  else if(class == POS_INF){
    s[0] = 'i';
    s[1] = 'n';
    s[2] = 'f';
    s[3] = '\0';
    return 3;
  }
  else if(class == NEG_INF){
    s[0] = '-';
    s[1] = 'i';
    s[2] = 'n';
    s[3] = 'f';
    s[4] = '\0';
    return 4;
  }
  return 0;
}
