#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "conversion.h"
#include "formatting.h"



//local header

int int_fmt_d(int64_t a, char *str, uint16_t prec, uint8_t flags);

int int_fmt_u(uint64_t a, char *str, uint16_t prec, uint8_t flags);

int int_fmt_x(uint64_t a, char *str, uint16_t prec, uint8_t flags);

int int_fmt_o(uint64_t a, char *str, uint16_t prec, uint8_t flags);

int fp_fmt_e(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags);

int fp_fmt_f(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags);

int fp_fmt_g(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags);

int fp_special_case(fpclass_t class, char *str, uint8_t flags);



//string manipulation functions

static void str_rev(char *s, size_t size){
    if(size <= 1){
        return;
    }
    char *p = &s[size-1];
    while(s < p){
        *s ^= *p;
        *p ^= *s;
        *s ^= *p;
        s++;
        p--;
    }
}

static void shift_str(char *str, uint16_t n, uint16_t length){
    //shift string by n bytes to the right
    //supposes that there is sufficient memory allocated on buffer
    for(ssize_t i = (ssize_t)length-1; i >= 0; i--){
        str[i+n] = str[i];
    }
}

static void pad_str(char *str, uint16_t n, char c){
    for(uint16_t i = 0; i < n; i++){
        str[i] = c;
    }
}




//argument extraction functions

int convert_d(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
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
    else if(mods[0] == 'z' && mods[1] == '\0'){
        a = va_arg(ap, ssize_t);
    }
    else{
        a = va_arg(ap, int);
    }
    return int_fmt_d(a, str, prec, flags);
}

int convert_u(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
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
    else if(mods[0] == 'z' && mods[1] == '\0'){
        a = va_arg(ap, size_t);
    }
    else{
        a = va_arg(ap, unsigned int);
    }
    return int_fmt_u(a, str, prec, flags);
}

int convert_x(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
    uint64_t a;
    if(mods[0] == 'l' && mods[1] == '\0'){
        a = va_arg(ap, unsigned long);
    }
    else if(mods[0] == 'l' && mods[1] == 'l'){
        a = (uint64_t)va_arg(ap, unsigned long long);
    }
    else if(mods[0] == 'h' && mods[1] == '\0'){
        a = va_arg(ap, unsigned int);
    }
    else if(mods[0] == 'h' && mods[1] == 'h'){
        a = va_arg(ap, unsigned int);
    }
    else if(mods[0] == 'z' && mods[1] == '\0'){
        a = va_arg(ap, size_t);
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
    else if(mods[0] == 'z' && mods[1] == '\0'){
        a = va_arg(ap, size_t);
    }
    else{
        a = va_arg(ap, unsigned int);
    }
    return int_fmt_o(a, str, prec, flags);
}

int convert_p(va_list ap, char *str, uint16_t prec, uint8_t flags){
    //only FLAG_LADJ defined
    //conversion as #x
    uint8_t aux = flags|FLAG_ALTF;
    void *p = va_arg(ap, void*);
    return int_fmt_x((unsigned long long)p, str, prec, aux);
}

int convert_c(va_list ap, char *str){
    char c = (char)va_arg(ap, int);
    str[0] = c;
    str[1] = '\0';
    return 1;
}

int convert_s(va_list ap, char *str, uint16_t prec, uint8_t flags){
    char *s = va_arg(ap, char *);
    int l = 0;
    if(s == NULL){
        str[l++] = '(';
        str[l++] = 'n';
        str[l++] = 'u';
        str[l++] = 'l';
        str[l++] = 'l';
        str[l++] = ')';
        if(prec < l){
            l = prec;
            s[l] = '\0';
        }
    }
    else{
        if(flags&FLAG_PREC){
            while(s[l] && l < prec){
                str[l] = s[l];
                l++;
            }
        }
        else{
            while(s[l]){
                str[l] = s[l];
                l++;
            }
        }
    }
    s[l] = '\0';
    return l;
}

void convert_n(va_list ap, char mods[2], ssize_t total){
    if(mods[0] == 'l' && mods[1] == '\0'){
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
    else if(mods[0] == 'z' && mods[1] == '\0'){
        ssize_t *n = va_arg(ap, ssize_t*);
        *n = total;
    }
    else{
        int *n = va_arg(ap, int*);
        *n = total;
    }
}

int convert_e(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
    char s; //sign
    int32_t E; //binary exponent
    uint64_t m; //binary mantissa
    int32_t F; //decimal exponent
    uint64_t n; //decimal mantissa
    fpclass_t class;
    if(mods[0] == 'L' && mods[1] == '\0'){
        class = decomposeLongDouble(&s, &E, &m, va_arg(ap, long double));
    }
    else{
        class = decomposeDouble(&s, &E, &m, va_arg(ap, double));
    }
    if(class == NUMBER){
        if(m == 0){
            n = 0;
        }
        decimalConversion(&F, &n, E, m);
        return fp_fmt_e(str, s, n, F, prec, flags);
    }
    //otherwise: +inf, -inf or nan
    return fp_special_case(class, str, flags);
}

int convert_f(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
    char s; //sign
    int32_t E; //binary exponent
    uint64_t m; //binary mantissa
    int32_t F; //decimal exponent
    uint64_t n; //decimal mantissa
    fpclass_t class;
    if(mods[0] == 'L' && mods[1] == '\0'){
        class = decomposeLongDouble(&s, &E, &m, va_arg(ap, long double));
    }
    else{
        class = decomposeDouble(&s, &E, &m, va_arg(ap, double));
    }
    if(class == NUMBER){
        if(m == 0){
            n = 0;
        }
        decimalConversion(&F, &n, E, m);
        return fp_fmt_f(str, s, n, F, prec, flags);
    }
    //otherwise: +inf, -inf or nan
    return fp_special_case(class, str, flags);
}

int convert_g(va_list ap, char mods[2], char *str, uint16_t prec, uint8_t flags){
    char s; //sign
    int32_t E; //binary exponent
    uint64_t m; //binary mantissa
    int32_t F; //decimal exponent
    uint64_t n; //decimal mantissa
    fpclass_t class;
    if(mods[0] == 'L' && mods[1] == '\0'){
        class = decomposeLongDouble(&s, &E, &m, va_arg(ap, long double));
    }
    else{
        class = decomposeDouble(&s, &E, &m, va_arg(ap, double));
    }
    if(class == NUMBER){
        if(m == 0){
            n = 0;
        }
        decimalConversion(&F, &n, E, m);
        return fp_fmt_g(str, s, n, F, prec, flags);
    }
    //otherwise: +inf, -inf or nan
    return fp_special_case(class, str, flags);
}




//integer formatting functions

int int_fmt_d(int64_t a, char *str, uint16_t prec, uint8_t flags){
    if(!(flags&FLAG_PREC)){
        prec = 1;
    }
    int i = 0;
    if(a == 0){
        if(flags&FLAG_SIGN){
            str[i++] = '+';
        }
        else if(flags&FLAG_WSPC){
            str[i++] = ' ';
        }
        int j = 0;
        while(j++ < prec){
            str[i++] = '0';
        }
        str[i] = '\0';
        return i;
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
    while(b != 0){
        str[i++] = '0'+b%10;
        b /= 10;
    }
    while(i < prec){
        str[i++] = '0';
    }
    if(neg){
        str[i++] = '-';
    }
    else if(flags&FLAG_SIGN){
        str[i++] = '+';
    }
    else if(flags&FLAG_WSPC){
        str[i++] = ' ';
    }
    str[i] = '\0';
    str_rev(str, i);
    return i;
}

int int_fmt_u(uint64_t a, char *str, uint16_t prec, uint8_t flags){
    if(!(flags&FLAG_PREC)){
        prec = 1;
    }
    if(a == 0){
        int j = 0;
        while(j < prec){
            str[j++] = '0';
        }
        str[j] = '\0';
        return j;
    }
    int i = 0;
    while(a != 0){
        str[i++] = '0'+a%10;
        a /= 10;
    }
    while(i < prec){
        str[i++] = '0';
    }
    str[i] = '\0';
    str_rev(str, i);
    return i;
}

int int_fmt_x(uint64_t a, char *str, uint16_t prec, uint8_t flags){
    //if no precision specified, precision is 1
    if(!(flags&FLAG_PREC)){
        prec = 1;
    }
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
        if(h < 10){
            str[i++] = '0'+h;
        }
        else{
            str[i++] = h-10+tmp_c;
        }
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
    if(!(flags&FLAG_PREC)){
        prec = 1;
    }
    if(a == 0){
        int i = 0;
        if(prec == 0 && (flags&FLAG_ALTF)){
            //there must be at least one zero
            str[i++] = '0';
        }
        else{
            while(i < prec){
                str[i++] = '0';
            }
        }
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
    if(i < prec){
        //pad with zeros
        while(i < prec){
            str[i++] = '0';
        }
    }
    else{
        if(flags&FLAG_ALTF){
            //there must be at leat one zero
            str[i++] = '0';
        }
    }
    str[i] = '\0';
    str_rev(str, i);
    return i;
}


//floating point formatting functions

int fp_fmt_e(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags){
    //adjust precision
    if(flags&FLAG_PREC){
        if(prec > 18){
            prec = 18;
        }
    }
    else{
        //no precision specified, taken as 6
        prec = 6;
    }

    //write sign
    int i = 0;
    if(s){
        str[i++] = '-';
    }
    else if(flags&FLAG_SIGN){
        str[i++] = '+';
    }
    else if(flags&FLAG_WSPC){
        str[i++] = ' ';
    }

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
    int_fmt_u(n,d,0,0);
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
        int_fmt_d(F,e,0,0);
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
    if(e[j] == '-'){
        str[i++] = e[j++];
    }
    else{
        str[i++] = '+';
    }
    while(e[j]){
        str[i++] = e[j++];
    }
    str[i] = '\0';
    return i;
}

int fp_fmt_f(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags){
    //adjust precision
    if(flags&FLAG_PREC){
        if(prec > 19){
            prec = 19;
        }
    }
    else{
        //no precision specified, taken as 6
        prec = 6;
    }

    //write sign
    int i = 0;
    if(s){
        str[i++] = '-';
    }
    else if(flags&FLAG_SIGN){
        str[i++] = '+';
    }
    else if(flags&FLAG_WSPC){
        str[i++] = ' ';
    }

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
    int_fmt_u(n,d,0,0);

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
                if(k < decimals){
                    str[i++] = d[j++];
                }
                else{
                    str[i++] = '0';
                }
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
                if(k < digits){
                    str[i++] = '0';
                }
                else{
                    str[i++] = d[j++];
                }
            }
        }
    }
    str[i] = '\0';
    return i;
}

int fp_fmt_g(char *str, char s, uint64_t n, int32_t F, uint16_t prec, uint8_t flags){
    //adjust precision
    if(flags&FLAG_PREC){
        if(prec > 19){
            prec = 19;
        }
        else if(prec == 0){
            prec = 1;
        }
    }
    else{
        //no precision specified, taken as 6
        prec = 6;
    }

    //write sign
    int i = 0;
    if(s){
        str[i++] = '-';
    }
    else if(flags&FLAG_SIGN){
        str[i++] = '+';
    }
    else if(flags&FLAG_WSPC){
        str[i++] = ' ';
    }

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
    int_fmt_u(n,d,0,0);

    //if written in 'e'-format, the exponent would be F+18
    if(F+18 >= (int)prec || F+18 < -4){
        //'e'-format conversion
        F += 18;
        int_fmt_d(F,e,0,0);
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
                if(str[i-1] == '.'){
                    i--;
                }
            }
        }
        int k = 0;
        str[i++] = 'e';
        if(e[k] == '-'){
            str[i++] = e[k++];
        }
        else{
            str[i++] = '+';
        }
        if(e[k] == '0'){
            //exponent is zero, has to be written as 00
            str[i++] = '0';
            str[i++] = '0';
        }
        else{
            if(F > -10 && F < 10){
                str[i++] = '0';
            }
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
            if(str[i-1] == '.'){
                i--;
            }
        }
    }
    str[i] = '\0';
    return i;
}

int fp_special_case(fpclass_t class, char *str, uint8_t flags){
    int i = 0;
    if(class == NAN){
        str[i++] = 'n';
        str[i++] = 'a';
        str[i++] = 'n';
        str[i] = '\0';
    }
    else if(class == POS_INF){
        if(flags&FLAG_SIGN){
            str[i++] = '+';
        }
        else if(flags&FLAG_WSPC){
            str[i++] = ' ';
        }
        str[i++] = 'i';
        str[i++] = 'n';
        str[i++] = 'f';
        str[i] = '\0';
    }
    else{
        //NEG_INF
        str[i++] = '-';
        str[i++] = 'i';
        str[i++] = 'n';
        str[i++] = 'f';
        str[i] = '\0';
    }
    return i;
}




//for padding after conversions

void pad_conversion(char fmt, char *str, uint8_t flags, uint16_t length, uint16_t field_width){
    //str contains the argument conversion
    //supposes that field_width > length
    int l = field_width-length;
    int integer = 0;
    char c = ' ';
    if(!(flags&FLAG_LADJ) && flags&FLAG_ZERO){
        c = '0';
    }
    switch(fmt){
        //integer conversions
        case 'i':
        case 'd':
        case 'u':
        case 'X':
        case 'x':
        case 'o':
        case 'p':
            if(flags&FLAG_PREC){
                //precision was given, ignore 0 flag
                c = ' ';
            }
            integer = 1;

        case 'E':
        case 'e':
        case 'F':
        case 'f':
        case 'G':
        case 'g':
            if(flags&FLAG_LADJ){
                //pad with whitespace on the right (left adjusted)
                pad_str(&str[length], l, c);
            }
            else{
                if(c == '0' && integer){
                    //pad with zeros on the left (right adjusted)
                    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
                        //0x... or 0X...
                        //padding will be 0x0000000...
                        //rather than     00000000x...
                        shift_str(&str[2], l, length-2);
                        pad_str(&str[2], l, '0');
                    }
                    else if(str[0] == ' ' || str[0] == '-' || str[0] == '+'){
                        //sign comes before the zeros
                        shift_str(&str[1], l, length-1);
                        pad_str(&str[1], l, '0');
                    }
                    else{
                        shift_str(str, l, length);
                        pad_str(str, l, '0');
                    }
                }
                else if(c == '0' && !integer){
                    //floating point padded with zeros
                    if(str[0] == 'n' || str[0] == 'i'){
                        //"nan" of "inf"
                        //pad with whitespace, not zeros
                        shift_str(str, l, length);
                        pad_str(str, l, ' ');
                    }
                    else if(str[0] == ' ' || str[0] == '-' || str[0] == '+'){
                        if(str[1] == 'i'){
                            //" inf" "-inf" or "+inf"
                            //pad with whitespace, not zeros
                            //and pad on the left
                            shift_str(&str[0], l, length);
                            pad_str(&str[0], l, ' ');
                        }
                        else{
                            shift_str(&str[1], l, length);
                            pad_str(&str[1], l, '0');
                        }
                    }
                    else{
                        shift_str(str, l, length);
                        pad_str(str, l, '0');
                    }
                }
                else{
                    //integer or floating point, c == ' '
                    //pad with whitespace on the left (right adjusted)
                    shift_str(str, l, length);
                    pad_str(str, l, ' ');
                }
            }
            break;

        case 'c':
        case 's':
            if(flags&FLAG_LADJ){
                pad_str(&str[length], l, c);
            }
            else{
                shift_str(str, l, length);
                pad_str(str, l, c);
            }
            break;

        default:
            break;
    }
    str[field_width] = '\0';
}
