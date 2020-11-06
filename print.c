#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include "print.h"
#include "formatting.h"

#define DFLT_SIZE 1024 //adjustable




int VSNPRINTF(char *str, size_t size, const char *fmt, va_list ap){
  unsigned int str_idx = 0; //where we are on the output string str
  unsigned int i = 0; //format index
  unsigned int k; //auxiliary when needed

  char tmp[384]; //argument conversions
  char mods[2]; //length modifiers
  uint16_t prec; //precision modifier
  uint8_t flags; //1 bit for each flag

  ssize_t length; //length of conversions
  ssize_t total = 0; //accumulator: total number of characters necessary
  unsigned int field_width; //minimum length, may require padding
  //char null_string[] = "(null)"; //for %s format with NULL argument

  //error cases
  if(size == 0)
    return -1;
  if(str == NULL)
    return -2;
  if(fmt == NULL)
    return -3;

  //main loop
  while(fmt[i] != '\0'){

    if(fmt[i] != '%'){
      //most commun case: character other than '%'
      // -> simply add it to str
      if(total < size-1)
        str[str_idx++] = fmt[i];
      total++;
      i++;
    }
    else{
      //this is where the fun begins...
      i++;

      if(fmt[i] == '%'){
        //jus wanted to print a '%' character, phew...
        if(total < size)
          str[str_idx++] = '%';
        i++;
        total++;
        continue;
      }

      //determine flags
      flags = 0;
      while(fmt[i] == '#' || fmt[i] == '0'|| fmt[i] == '-' || fmt[i] == ' ' || fmt[i] == '+'){
        switch (fmt[i]){
          case '#':
            flags |= FLAG_ALTF;
            break;
          case '0':
            flags |= FLAG_ZPAD;
            break;
          case '-':
            flags |= FLAG_LADJ;
            break;
          case ' ':
            flags |= FLAG_WSPC;
            break;
          case '+':
            flags |= FLAG_SIGN;
            break;
        }
        i++;
      } //end of flags

      //determine field width
      //if remains 0 no minimum requirement
      field_width = 0;
      if(fmt[i] >= '0' && fmt[i] <= '9'){
        flags |= FLAG_WDTH;
        while(fmt[i] >= '0' && fmt[i] <= '9'){
          field_width = field_width*10 + fmt[i++] - '0';
        }
      }

      //precision modifier
      if(fmt[i] == '.'){
        flags |= FLAG_PREC;
        prec = 0;
        i++;
        while(fmt[i] >= '0' && fmt[i] <= '9'){
          prec = prec*10 + (fmt[i++] - '0');
          if(prec > 40)
            prec = 40;
        }
      }

      //length modifiers
      mods[0] = '\0';
      mods[1] = '\0';
      k = 0;
      while(fmt[i] == 'l' || fmt[i] == 'h' || fmt[i] == 'L' || fmt[i] == 'z'){
        //absorb all length modifiers
        if(k++ < 2)
          mods[k] = fmt[i];
        i++;
      }
      if(k > 2){
        //there can never be more
        //than 2 length modifiers
        goto BAD_FORMAT;
      }

      char c;
      switch(fmt[i]){
        case 'i':
        case 'd':
          length = convert_d(ap, mods, tmp, flags);
          break;

        case 'u':
          length = convert_u(ap, mods, tmp, flags);
          break;

        case 'X':
          flags |= FLAG_UCAS;
        case 'x':
          length = convert_x(ap, mods, tmp, prec, flags);
          break;

        case 'o':
          length = convert_o(ap, mods, tmp, prec, flags);
          break;

        case 'n':
          convert_n(ap, mods, total);
          continue;

        case 'c':
          c = (char)va_arg(ap, int);
          length = 1;
          tmp[0] = c;
          tmp[1] = '\0';
          total++;
          break;

        case 'E':
          flags |= FLAG_UCAS;
        case 'e':
          length = convert_e(ap, mods, tmp, prec, flags);
          break;

        case 'F':
          flags |= FLAG_UCAS;
        case 'f':
          length = convert_f(ap, mods, tmp, prec, flags);
          break;

        case 'G':
          flags |= FLAG_UCAS;
        case 'g':
          length = convert_g(ap, mods, tmp, prec, flags);
          break;

        default:
          goto BAD_FORMAT;
      } //end of switch statement

      total += length;
      if((ssize_t)size-length > total){
        for(k = 0; k < length; k++){
          str[str_idx++] = tmp[k];
        }
      }

      /*
      if(fmt[i] != 'c' && fmt[i] != 's'){
        //all cases except %c and %s
        //these two cases already printed to str

        if(tmp[0] == 'n'){
          //case nan
          if(field_width > length){
            if(flags&FLAG_LADJ){
              if(size-total >= field_width){
                for(k = 0; k < length; k++){
                  str[str_idx] = tmp[k];
                  str_idx++;
                }
                for(k = 0; k < field_width-length; k++){
                  str[str_idx] = ' ';
                  str_idx++;
                }
              }
            }
            else{
              if(size-total >= field_width){
                for(k = 0; k < field_width-length; k++){
                  str[str_idx] = ' ';
                  str_idx++;
                }
                for(k = 0; k < length; k++){
                  str[str_idx] = tmp[k];
                  str_idx++;
                }
              }
            }
            total += field_width;
          }
          else{
            if(size-total >= length){
              for(k = 0; k < length; k++){
                str[str_idx] = tmp[k];
                str_idx++;
              }
            }
            total += length;
          }
        }

        else if(length < field_width){
          if(Padding_F == FLAG_LADJ){
            if(tmp[0] != '-' && (flags&FLAG_WSPC || flags&FLAG_SIGN)){
              if(size > total){
                if(flags&FLAG_SIGN)
                  str[str_idx++] = '+';
                else
                  str[str_idx++] = ' ';
              }
              total++;
              field_width--;
            }
            if(size-total >= field_width){
              k = 0;
              while(tmp[k] != '\0'){
                str[str_idx] = tmp[k];
                str_idx++;
                k++;
              }
              for(k = 0; k < field_width-length; k++){
                str[str_idx] = ' ';
                str_idx++;
              }
            }
            total += field_width;
            //end of case Padding_F == RIGHT_PADDED
          }

          else if(Padding_F == SPACE_PADDED || tmp[0] == 'i' || tmp[1] == 'i'){
            //also case of +/-inf
            if(size-total >= field_width-length){
              for(k = 0; k < field_width-length; k++){
                str[str_idx] = ' ';
                str_idx++;
              }
            }
            if(Sign_F == SIGN && tmp[0] != '-'){
              if(str_idx-1 < size){
                str[str_idx-1] = '+';
              }
            }
            total += field_width - length;
            if(size-total >= length){
              k = 0;
              while(tmp[k] != '\0'){
                str[str_idx] = tmp[k];
                str_idx++;
                k++;
              }
            }
            total += length;
          }

          else{
            //case Padding_F == ZERO_PADDED
            if(Sign_F != NO_SIGN && tmp[0] != '-'){
              if(size > total){
                if(Sign_F == BLANK) str[str_idx] = ' ';
                else str[str_idx] = '+';
                str_idx++;
              }
              total++;
              field_width--;
            }
            if(size-total >= field_width-length){
              for(k = 0; k < field_width-length; k++){
                str[str_idx] = '0';
                str_idx++;
              }
            }
            total += field_width - length;
            if(size-total >= length){
              k = 0;
              while(tmp[k] != '\0'){
                str[str_idx] = tmp[k];
                str_idx++;
                k++;
              }
            }
            total += length;
          }
          //end case length < field_width
        }

        else{
          //no padding necessary
          if(Sign_F != NO_SIGN && tmp[0] != '-'){
            total++;
            if(size > total-1){
              if(Sign_F == BLANK){
                str[str_idx] = ' ';
              }
              else{
                //case Sign_F == SIGN
                str[str_idx] = '+';
              }
              str_idx++;
            }
          }
          if(size - total >= length){
            k = 0;
            while(tmp[k] != '\0'){
              str[str_idx] = tmp[k];
              str_idx++;
              k++;
            }
          }
          total += length;
          //end case length >= field_width
        }

      }
      */
      i++;
      continue; //back to main 'while' loop

      BAD_FORMAT:
      if(fmt[i] != '\0'){
        if((ssize_t)size-total > 1){
          //room for (at least) one more character
          //before terminating null byte
          str[str_idx++] = fmt[i];
        }
        total++;
        i++;
      }
      else{
        str[str_idx] = '\0';
        return total;
      }
    } //end of if(fmt[i] == '%')

  } //end of main 'while' loop

  str[str_idx] = '\0';
  return total;

  //returns number of characters necessary
  //in case of insufficient size on str

  //NOTES:
  //if(str_idx == size)
  //means that str is completely full
  //added '\0' will be added in calling function
  //default size for vsnprintf call is DFLT_SIZE
}



//derivatives

int VSPRINTF(char *str, const char *format, va_list ap){
  int res;
  res = VSNPRINTF(str, SIZE_MAX, format, ap);
  return res;
}

int VPRINTF(const char *format, va_list ap){
  char buff[DFLT_SIZE];
  int res;
  va_list aq;
  va_copy(aq,ap);
  res = VSNPRINTF(buff, DFLT_SIZE, format, ap);
  if(res >= DFLT_SIZE){
    char *str = (char *)alloca((res+1)*sizeof(char));
    VSNPRINTF(str, res+1, format, aq);
    PUTS(str);
  }
  else{
    PUTS(buff);
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
  va_start(ap, format);
  int res = VSPRINTF(str, format, ap);
  va_end(ap);
  return res;
}

int PRINTF(const char *format, ...){
  va_list ap;
  va_start(ap, format);
  int res = VPRINTF(format, ap);
  va_end(ap);
  return res;
}
