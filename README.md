# Overview
A small printf function (and some variants) using only small system calls (such as putchar(), ... no malloc() for instance) and that can output floatinf point numbers without using floating point operations. This project is intended for use in boot mode (or whenever you don't have access to libc functions), or on systems that need to print floating point numbers but have no floating point computation ability for the binary-to-decimal conversion.
This was originally a university project I did with my classmate Bijan Rad at the Faculty of Sciences of Sorbonne University, Paris.
I recently revisited this project and am in the process of cleaning up the code.
The file <algorithm.pdf> explains the mathematics behind the conversion algorithm.

# Functions and system calls
The only system calls used are putchar() and alloca(). The code supposes that you can define these two functions as someting other than a system call in your target environment. The code also requires that you have access to the "va_list" type, as well as the va_start(), va_end(), va_arg() macros.
The following functions are implemented:
- vsnprintf()
- vsprintf()
- vprintf()
- snprintf()
- sprintf()
- printf()

Note that these are declared in upper case letters in the code (e.g. VSNPRINTF) in <print.c> and <print.h>. This was in order to develop and test them without conflict with the standard C library.
The heart of the project is the vsnprintf() function. All of the other functions are essentially wrappers for it.
The functions fprintf and vfprintf are not implemented, as these would require a fputc() and fputs() to be defined in the target environment. Again, if you are able to define a fputc() function in your target environment (and also, therefore, a fputs() function), you can easily implement fprintf() and vfprintf() as wrappers to the implemented vsnprintf() function.
The functions asprintf() and vasprintf() are not implemented: through their argument "ret", they return a pointer to dynamically allocated memory containing the formatted string. The problem is that in our context, we cannot use the system call malloc().

# Floating point numbers
Variables or constants of type "float" are converted to type "double" implicitly.
This the aforementioned printf functions can print both "double" and long "double" arguments (via the 'L' modifier).
Note that "double" is taken to be a IEEE-754 64-bit floating point number, and long "double" is taken to be a IEEE-754 80-bit extended precision floating point number that is padded with zeros on the left up to 128 bits. I fthis is not the case in your context, you can edit the decomposeDouble() and decomposeLongDouble() functions in <conversion.c>.
FLoating point conversions are precise up to the 17th digit after the decimal point if you were to write them in scientific notation.
Percision for floating point conversions id capped at 18 or 19 depending on the format so as not to show more than 19 significant digits, as the conversion algorithm used cannot give us more than that.
The conversion algorithm is explained given in the file <algorithm.pdf>.

# Compilation
The algorithm used to convert floating point number uses 128-bit unsigned integers provided by GCC's "__uint128_t" type.

# Ongoing work
Implemented formats  d, u, x, o, n, c, e, E, f, F, g, F  modifiers l, ll, h, hh, L  and the  #  flag.
Working on formats  a, A, s, p  modifiers j, z, t and flags  0, +, -, and ' ' (whitespace).
The padding flags  0', '+', '-', and ' '  can be specified in the format string, they just don't do anything yet.
Same for the field width modifier, it can be specified in the format string, but doesn't do anything yet.
The precision modifier is implemented for floating point conversions, but not yet for all integer conversions.
Right now, besides floating point conversions, precision is capped at 40.
Also still cleaning up the code.
Of course, there are probably still some bugs...


All rights reserved.
