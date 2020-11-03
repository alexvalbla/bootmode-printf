#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include "print.h"
#include "auxFunctions.h"
#define SIZE 1024 //adjustable if need be



// printing functions

int VSNPRINTF(char *str, size_t size, const char *format, va_list ap){
  unsigned int i = 0;//format
  unsigned int strIndex = 0;//str (nb characters printed so far)
  unsigned int k;//auxiliary when needed
  unsigned int decimal;//floating point formats
  unsigned int acc = 0;//for necessary nb of characters
  char tmp[40];//arg conversions
  char arg[5];//format modifiers

  //FLAGS:
  P_mode Padding_F;//'0' and '-'
  S_mode Sign_F;//' ' and '+'
  unsigned int length=0;//length of conversions
  unsigned int fieldW;//minimum length (padding?)
  char null_array[] = "(null)";

  //in case o: "goto bad_format;"
  char error_message[40] = "Error: unknown format: ";
  unsigned int t;
  unsigned int l;
  //bad_format instructions at the bottom


  //exit cases
  if(str == NULL && size != 0){
    char err[] = "Error: cannot write on a null string.\n";
    print_string(err);
    return 0;
  }
  if(format == NULL){
    char err[] = "Error: format not specified.\n";
    print_string(err);
    return 0;
  }

  //START MAIN LOOP
  while(format[i] != '\0'){
    BIG_LOOP:

    if(format[i] == '%'){
      i++;

      if(format[i] == '%'){
        if(acc < size){
          str[strIndex] = '%';
          strIndex++;
        }
        i++;
        acc++;
        goto BIG_LOOP;
      }

      else if(format[i] == 'n'){
        int *n = va_arg(ap, int *);
        *n = strIndex;
        i++;
        goto BIG_LOOP;
      }
      else if(format[i] == 'l'){
        if(format[i+1] == 'n'){
          long *n = va_arg(ap, long *);
          *n = strIndex;
          i += 2;
          goto BIG_LOOP;
        }
        else if(format[i+1] == 'l'){
          if(format[i+2] == 'n'){
            long long *n = va_arg(ap, long long *);
            *n = strIndex;
            i += 3;
            goto BIG_LOOP;
          }
        }
      }

      //Determine flags
      Padding_F = SPACE_PADDED;
      Sign_F = NO_SIGN;

      while(format[i] == '0'|| format[i] == '-' || format[i] == ' ' || format[i] == '+'){
        if(format[i] == '-'){
          Padding_F = RIGHT_PADDED;
        }
        else if(Padding_F != RIGHT_PADDED && format[i] == '0'){
          Padding_F = ZERO_PADDED;
          //note that - overrides 0 flag
        }
        else if(format[i] == '+'){
          Sign_F = SIGN;
        }
        else if(Sign_F != SIGN && format[i] == ' '){
          Sign_F = BLANK;
          //note that + overrides ' ' flag
        }
        i++;
      }
      //end of flags

      //determine field width
      //if remains 0 no minimum requirement
      fieldW = 0;
      while(format[i] >= '0' && format[i] <= '9'){
        fieldW = fieldW*10 + format[i] - '0';
        //fieldW = (fieldW<<3) + (fieldW<<1) + format[i] - '0';//faster?
        i++;
      }

      //number of decimals in case of floating points
      decimal = 6;//default
      if(format[i] == '.'){
        //decimal modifier for floating points
        i++;
        decimal = 0;
        while(format[i] >= '0' && format[i] <= '9'){
          decimal = decimal*10 + format[i] - '0';
          //decimal = (decimal<<3) + (decimal<<1) + format[i] - '0';//faster?
          i++;
        }
        if(decimal > 18) decimal = 18;
      	if(decimal == 0) decimal = 1;
        //end of .decimal extraction
      }
      //note that decimal always between 0 and 18
      //no need for further check


      //Determine formats
      k = 0;
      while((format[i] == 'l' || format[i] == 'h' || format[i] == 'L') && k < 3){
        //absorb all length modifiers
        arg[k] = format[i];
        i++;
        k++;
      }
      //i index now resting on u,d,o,x,e,f,g,s, or c

      if(k==0){
        //no length modifiers
        if(format[i] == 'u'){
          length = string_u(va_arg(ap, unsigned int), tmp);
        }
        else if(format[i] == 'd'){
          length = string_d(va_arg(ap, int), tmp);
        }
        else if(format[i] == 'o'){
          length = string_o(va_arg(ap, unsigned int), tmp);
        }
        else if(format[i] == 'x'){
          length = string_x(va_arg(ap, unsigned int), tmp, 0);
        }
        else if(format[i] == 'X'){
          length = string_x(va_arg(ap, unsigned int), tmp, 1);
        }
        else if(format[i] == 'e'){
          length = string_e(va_arg(ap, double), tmp, decimal);
        }
        else if(format[i] == 'f'){
          length = string_f(va_arg(ap, double), tmp, decimal);
        }
        else if(format[i] == 'g'){
          length = string_g(va_arg(ap, double), tmp, decimal);
        }
        else if(format[i] == 's'){
          char *s = va_arg(ap, char*);
          if(s == NULL){
            s = null_array;
            length = 6;
          }
          else{
            length = 0;
            while(s[length] != '\0') length++;
          }
          unsigned int padding_length;
          if(fieldW > length) padding_length = fieldW - length;
          else padding_length = 0;
          if(size - acc >= length + padding_length){
            //if room to print to str
            string_s(s, str, strIndex, Padding_F, length, padding_length);
            strIndex += length + padding_length;
          }
          acc += length + padding_length;//in any case
        }
        else if(format[i] == 'c'){
          if(size > acc){
            char c = (char)va_arg(ap, int);
            str[strIndex] = c;
            strIndex++;
          }
          acc++;
        }
        else{
          goto BAD_FORMAT;
        }
        //end case no modifier
      }

      else if(k==1){
        //single length modifier
        if(arg[0] == 'l'){
          //cases: u,d,o,x,X
          if(format[i] == 'u'){
            length = string_lu(va_arg(ap, unsigned long), tmp);
          }
          else if(format[i] == 'd'){
            length = string_ld(va_arg(ap, long), tmp);
          }
          else if(format[i] == 'o'){
            length = string_lo(va_arg(ap, unsigned long), tmp);
          }
          else if(format[i] == 'x'){
            length = string_lx(va_arg(ap, unsigned long), tmp, 0);
          }
          else if(format[i] == 'X'){
            length = string_lx(va_arg(ap, unsigned long), tmp, 1);
          }
          else{
            goto BAD_FORMAT;
          }
          //end case l modifier
        }

        else if(arg[0] == 'L'){
          //long double cases
          if(format[i] == 'e'){
            length = string_Le(va_arg(ap, long double), tmp, decimal);
          }
          else if(format[i] == 'f'){
            length = string_Lf(va_arg(ap, long double), tmp, decimal);
          }
          else if(format[i] == 'g'){
            length = string_Lg(va_arg(ap, long double), tmp, decimal);
          }
          else{
            goto BAD_FORMAT;
          }
          //end case L modifier
        }

        else if(arg[0] == 'h'){
          //case 'h': u,d,o,x,X
          if(format[i] == 'u'){
            length = string_u(va_arg(ap, unsigned int), tmp);
          }
          else if(format[i] == 'd'){
            length = string_d(va_arg(ap, int), tmp);
          }
          else if(format[i] == 'o'){
            length = string_o(va_arg(ap, unsigned int), tmp);
          }
          else if(format[i] == 'x'){
            length = string_x(va_arg(ap, unsigned int), tmp, 0);
          }
          else if(format[i] == 'X'){
            length = string_x(va_arg(ap, unsigned int), tmp, 1);
          }
          else{
            goto BAD_FORMAT;
          }
          //end case h modifier
        }
        //end case single modifier
      }

      else if(k==2){
        //only valid for integers
        //long double -> %Le or %Lf
        if(arg[0] == 'l' && arg[1] == 'l'){
          if(format[i] == 'u'){
            length = string_llu(va_arg(ap, unsigned long long), tmp);
          }
          else if(format[i] == 'd'){
            length = string_lld(va_arg(ap, long long), tmp);
          }
          else if(format[i] == 'o'){
            length = string_llo(va_arg(ap, unsigned long long), tmp);
          }
          else if(format[i] == 'x'){
            length = string_llx(va_arg(ap, unsigned long long), tmp, 0);
          }
          else if(format[i] == 'X'){
            length = string_llx(va_arg(ap, unsigned long long), tmp, 1);
          }
          else{
            goto BAD_FORMAT;
          }
          //end case ll modifier
        }

        else if(arg[0] == 'h' && arg[1] == 'h'){
          //case u, d
          if(format[i] == 'u'){
            length = string_u(va_arg(ap, unsigned int), tmp);
          }
          else if(format[i] == 'd'){
            length = string_d(va_arg(ap, int), tmp);
          }
          else{
            goto BAD_FORMAT;
          }
          //end case hh modifier
        }

        else{
          //cases like: lh, lh...
          goto BAD_FORMAT;
        }
        //end case double modifiers
      }

      else{
        //k > 2: too many length modifiers
        goto BAD_FORMAT;
      }
      //end of formats


      if(!(format[i] == 'c' || format[i] == 's')){
        //note the ! operator
        //all cases except %c and %s
        //these two cases already printed to str

        if(tmp[0] == 'n'){
          //case nan
          if(fieldW > length){
            if(Padding_F == RIGHT_PADDED){
              if(size-acc >= fieldW){
                for(k = 0; k < length; k++){
                  str[strIndex] = tmp[k];
                  strIndex++;
                }
                for(k = 0; k < fieldW-length; k++){
                  str[strIndex] = ' ';
                  strIndex++;
                }
              }
            }
            else{
              if(size-acc >= fieldW){
                for(k = 0; k < fieldW-length; k++){
                  str[strIndex] = ' ';
                  strIndex++;
                }
                for(k = 0; k < length; k++){
                  str[strIndex] = tmp[k];
                  strIndex++;
                }
              }
            }
            acc += fieldW;
          }
          else{
            if(size-acc >= length){
              for(k = 0; k < length; k++){
                str[strIndex] = tmp[k];
                strIndex++;
              }
            }
            acc += length;
          }
        }

        else if(length < fieldW){
          if(Padding_F == RIGHT_PADDED){
            if(Sign_F != NO_SIGN && tmp[0] != '-'){
              if(size > acc){
                if(Sign_F == SIGN){
                  str[strIndex] = '+';
                }
                else{
                  //case Sign_F == BLANK
                  str[strIndex] = ' ';
                }
                strIndex++;//adjust for added + or ' '
              }
              acc++;
              fieldW--;
            }
            if(size-acc >= fieldW){
              k = 0;
              while(tmp[k] != '\0'){
                str[strIndex] = tmp[k];
                strIndex++;
                k++;
              }
              for(k = 0; k < fieldW-length; k++){
                str[strIndex] = ' ';
                strIndex++;
              }
            }
            acc += fieldW;
            //end of case Padding_F == RIGHT_PADDED
          }

          else if(Padding_F == SPACE_PADDED || tmp[0] == 'i' || tmp[1] == 'i'){
            //also case of +/-inf
            if(size-acc >= fieldW-length){
              for(k = 0; k < fieldW-length; k++){
                str[strIndex] = ' ';
                strIndex++;
              }
            }
            if(Sign_F == SIGN && tmp[0] != '-'){
              if(strIndex-1 < size){
                str[strIndex-1] = '+';
              }
            }
            acc += fieldW - length;
            if(size-acc >= length){
              k = 0;
              while(tmp[k] != '\0'){
                str[strIndex] = tmp[k];
                strIndex++;
                k++;
              }
            }
            acc += length;
          }

          else{
            //case Padding_F == ZERO_PADDED
            if(Sign_F != NO_SIGN && tmp[0] != '-'){
              if(size > acc){
                if(Sign_F == BLANK) str[strIndex] = ' ';
                else str[strIndex] = '+';
                strIndex++;
              }
              acc++;
              fieldW--;
            }
            if(size-acc >= fieldW-length){
              for(k = 0; k < fieldW-length; k++){
                str[strIndex] = '0';
                strIndex++;
              }
            }
            acc += fieldW - length;
            if(size-acc >= length){
              k = 0;
              while(tmp[k] != '\0'){
                str[strIndex] = tmp[k];
                strIndex++;
                k++;
              }
            }
            acc += length;
          }
          //end case length < fieldW
        }

        else{
          //no padding necessary
          if(Sign_F != NO_SIGN && tmp[0] != '-'){
            acc++;
            if(size > acc-1){
              if(Sign_F == BLANK){
                str[strIndex] = ' ';
              }
              else{
                //case Sign_F == SIGN
                str[strIndex] = '+';
              }
              strIndex++;
            }
          }
          if(size - acc >= length){
            k = 0;
            while(tmp[k] != '\0'){
              str[strIndex] = tmp[k];
              strIndex++;
              k++;
            }
          }
          acc += length;
          //end case length >= fieldW
        }

        //end case floats or integers
      }

      //end of if(format[i] == '%')
    }

    else{
      //case character other than '%' -> simply print to str
      if(strIndex < size){
        str[strIndex] = format[i];
        strIndex++;
      }
      acc++;
    }

    i++;
    //end of while not '\0' loop
  }

  str[strIndex] = '\0';
  return acc;
  //returns number of characters necessary
  //in case of insufficient size on str

  //NOTES:
  //if(strIndex == size)
  //means that str is completely full
  //added '\0' will be added in calling function
  //default size for vsnprintf call is 1000

  BAD_FORMAT:
  t = 23;//next index on error_message
  l = 0;
  error_message[t] = '%';
  t++;
  while(l < k){
    error_message[t] = arg[l];
    t++;
    l++;
  }
  error_message[t] = format[i];
  error_message[t+1] = '\n';
  error_message[t+2] = '\0';
  print_string(error_message);
  str[0] = '\0';
  return 0;
}



//derivatives

int VSPRINTF(char *str, const char *format, va_list ap){
  int res;
  res = VSNPRINTF(str, UINT_MAX, format, ap);//
  return res;
}

int VFPRINTF(FILE *stream, const char *format, va_list ap){
  if(stream == NULL){
    char err[] = "Error: cannot write on a NULL file or stream.\n";
    print_string(err);
    return 0;
  }
  int res;
  char buff[SIZE];
  va_list aq;
  va_copy(aq,ap);
  res = VSNPRINTF(buff, SIZE, format, ap);
  if(res>=SIZE){
    char *str = (char *)alloca((res+1)*sizeof(char));
    VSNPRINTF(str, res+1, format, aq);
    fputs(str, stream);
  }
  else{
    fputs(buff, stream);
  }
  va_end(aq);
  return res;
}

int VPRINTF(const char *format, va_list ap){
  char buff[SIZE];
  int res;
  va_list aq;
  va_copy(aq,ap);
  res = VSNPRINTF(buff, SIZE, format, ap);
  if(res>=SIZE){
    char *str = (char *)alloca((res+1)*sizeof(char));
    VSNPRINTF(str, res+1, format, aq);
    print_string(str);
  }
  else{
    print_string(buff);
  }
  va_end(aq);
  return res;
}

int SNPRINTF(char *str, size_t size, const char *format, ...){
  va_list ap;
  va_start(ap, format);
  int res = VSNPRINTF(str, size, format, ap);
  va_end(ap);
  return res;
}

int SPRINTF(char *str, const char *format, ...){
  va_list ap;
  int res;
  va_start(ap, format);
  res = VSPRINTF(str, format, ap);
  return res;
}

int FPRINTF(FILE *stream, const char *format, ...){
  int res;
  va_list ap;
  va_start(ap, format);
  res = VFPRINTF(stream, format, ap);
  va_end(ap);
  return res;
}

int PRINTF(const char *format, ...){
  int res;
  va_list ap;
  va_start(ap, format);
  res = VPRINTF(format, ap);
  va_end(ap);
  return res;
}
