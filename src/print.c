#include "formatting.h"


//adjustable if needed
#define DFLT_SIZE 1024
#define MAX_PREC 300
#define MAX_FIELD_WIDTH 512


// typedef struct {
//         void *output_ptr; // cast as either (char *) of (FILE *) for fputc
//         char conv_buff[DFLT_SIZE];
//         char pref_buff[16];
//         char suff_buff[16];
//         char padd_char; //either ' '(space) or '0'
//         char *string_ptr; //when converting %s
//         size_t n; //limit on the number of characters to be written
//         uint16_t flags;
// } bm_output_context;


static inline void output_to_stdout(char *str){
    //unlike puts(), does not append '\n' character
    while(*str){
        putchar(*(str++));
    }
}

int bootmode_vsnprintf(char *str, size_t size, const char *fmt, bm_va_list ap){
    size_t str_idx = 0; //where we are on the output string str
    size_t i = 0; //format string index

    char tmp[MAX_FIELD_WIDTH+1]; //buffer for argument conversions
    char mods[2]; //length modifiers
    uint8_t flags; //1 bit for each flag
    uint16_t prec; //precision modifier
    uint16_t field_width; //minimum length, may require padding
    uint16_t length; //length of argument conversions
    size_t total = 0; //accumulator: total number of characters necessary

    //error cases
    if(size == 0){
        return -1;
    }
    if(str == NULL){
        return -2;
    }
    if(fmt == NULL){
        return -3;
    }

    //main loop
    while(fmt[i] != '\0'){

        if(fmt[i] != '%'){
            //most common case: character other than '%'
            //simply append it to str
            total++;
            if(total < size){
                str[str_idx++] = fmt[i];
            }
            i++;
        }
        else{
            //this is where the fun begins...
            i++;

            //mark flags
            flags = 0;
            while(fmt[i] == '#' || fmt[i] == '0'|| fmt[i] == '-' || fmt[i] == ' ' || fmt[i] == '+'){
                switch (fmt[i]){
                    case '#':
                        flags |= FLAG_ALTF;
                        break;
                    case '0':
                        flags |= FLAG_ZERO;
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
            }

            //field width modifier
            field_width = 0;
            if(fmt[i] >= '1' && fmt[i] <= '9'){
                flags |= FLAG_WDTH;
                while(fmt[i] >= '0' && fmt[i] <= '9'){
                    field_width = field_width*10 + fmt[i++] - '0';
                    if(field_width > MAX_FIELD_WIDTH){
                        field_width = MAX_FIELD_WIDTH;
                    }
                }
            }

            //precision modifier
            prec = 0;
            if(fmt[i] == '.'){
                flags |= FLAG_PREC;
                i++;
                while(fmt[i] >= '0' && fmt[i] <= '9'){
                    prec = prec*10 + (fmt[i++] - '0');
                    if(prec > MAX_PREC){
                        prec = MAX_PREC;
                    }
                }
            }

            //length modifiers
            //reset previous conversion modifiers:
            mods[0] = '\0';
            mods[1] = '\0';
            int k = 0;
            while(fmt[i] == 'l' || fmt[i] == 'h' || fmt[i] == 'L' || fmt[i] == 'z'){
                //absorb all length modifiers
                if(k < 2){
                    //normally, no more than 2 length modifiers
                    //e.g. lld, hhu,... for a correct format specification
                    //otherwise, only the first 2 count
                    //e.g. hhl -> hh
                    mods[k] = fmt[i];
                }
                k++;
                i++;
            }

            //argument conversion
            //during conversions, incorrect 2-length modifiers...
            //e.g. lh, hz, etc...
            //or undefined length modifiers for the conversion specifier...
            //e.g. Ld, zf, etc...
            //... will be ignored
            switch(fmt[i]){
                case 'i':
                case 'd':
                    length = convert_d(ap, mods, tmp, prec, flags);
                    break;

                case 'u':
                    length = convert_u(ap, mods, tmp, prec, flags);
                    break;

                case 'X':
                    flags |= FLAG_UCAS;
                    // fall through
                case 'x':
                    length = convert_x(ap, mods, tmp, prec, flags);
                    break;

                case 'o':
                    length = convert_o(ap, mods, tmp, prec, flags);
                    break;

                case 'p':
                    length = convert_p(ap, tmp, prec, flags);
                    break;

                case 'E':
                    flags |= FLAG_UCAS;
                    // fall through
                case 'e':
                    length = convert_e(ap, mods, tmp, prec, flags);
                    break;

                case 'F':
                    flags |= FLAG_UCAS;
                    // fall through
                case 'f':
                    length = convert_f(ap, mods, tmp, prec, flags);
                    break;

                case 'G':
                    flags |= FLAG_UCAS;
                    // fall through
                case 'g':
                    length = convert_g(ap, mods, tmp, prec, flags);
                    break;

                case 'c':
                    length = convert_c(ap, tmp);
                    break;

                case 's':
                    length = convert_s(ap, tmp, prec, flags);
                    break;

                case 'n':
                    convert_n(ap, mods, total);
                    i++;
                    continue;

                case '%':
                    total++;
                    if(total < size){
                        str[str_idx++] = '%';
                    }
                    i++;
                    continue;

                default:
                    goto BAD_FORMAT;
            }

            if((flags&FLAG_WDTH) && length < field_width){
                pad_conversion(fmt[i], tmp, flags, length, field_width);
                length = field_width;
            }
            tmp[length] = '\0'; //should already be there, but just in case...

            //remember: need to reserve 1 byte for '\0' character
            //total is the number of character needed so far
            //BEFORE appending the new conversion
            if(total+1 < size){
                size_t space_left = size-total-1;
                if(total+length < size){
                    //space to copy the whole conversion
                    for(size_t i = 0; i < length; i++){
                        str[str_idx++] = tmp[i];
                    }
                }
                else{
                    //copy what you have space for
                    for(size_t i = 0; i < space_left; i++){
                        str[str_idx++] = tmp[i];
                    }
                }
            }
            total += length;
            i++; //point to next fmt character
            continue; //back to beginning of main 'while' loop

            BAD_FORMAT:
            if(fmt[i] != '\0'){
                total++;
                if(total < size){
                    //room for (at least) one more character
                    //before terminating null byte
                    str[str_idx++] = fmt[i];
                }
                i++;
            }
            else{
                str[str_idx] = '\0';
                return (int)total;
            }
        } //end of 'if(fmt[i] == '%')'

    } //end of main 'while' loop

    str[str_idx] = '\0';
    return (int)total;

    //NOTES:
    //writes at most size-1 bytes to str
    //always appends a '\0' charater
    //if total >= size, then not verything was written
    //returns number of characters necessary...
    //...if str had had sufficient memory
}



//derivatives

int bootmode_vsprintf(char *str, const char *format, bm_va_list ap){
    int res;
    res = bootmode_vsnprintf(str, SIZE_MAX, format, ap);
    return res;
}

int bootmode_vprintf(const char *format, bm_va_list ap){
    char buff[DFLT_SIZE];
    int res;
    bm_va_list aq;
    bm_va_copy(aq,ap);
    res = bootmode_vsnprintf(buff, DFLT_SIZE, format, ap);
    if(res >= DFLT_SIZE){
        char *str = (char *)alloca((res+1)*sizeof(char));
        bootmode_vsnprintf(str, res+1, format, aq);
        output_to_stdout(str);
    }
    else{
        output_to_stdout(buff);
    }
    bm_va_end(aq);
    return res;
}

int bootmode_snprintf(char *str, size_t size, const char *format, ...){
    bm_va_list ap;
    bm_va_start(ap, format);
    int res = bootmode_vsnprintf(str, size, format, ap);
    bm_va_end(ap);
    return res;
}

int bootmode_sprintf(char *str, const char *format, ...){
    bm_va_list ap;
    bm_va_start(ap, format);
    int res = bootmode_vsprintf(str, format, ap);
    bm_va_end(ap);
    return res;
}

int bootmode_printf(const char *format, ...){
    bm_va_list ap;
    bm_va_start(ap, format);
    int res = bootmode_vprintf(format, ap);
    bm_va_end(ap);
    return res;
}
