# Overview
A small printf function (and some of the ususal variants) using only small system calls (such as putchar(), ... no malloc() for instance) and that can output floating point numbers without using floating point operations. This project is intended for use in boot mode (or whenever you don't have access to libc functions), or on systems that need to print floating point numbers but have no floating point unit for the binary-to-decimal conversion.
The file algorithm.pdf explains the mathematics behind the conversion algorithm.
This was originally a university project I did with my classmate Bijan Rad at the Faculty of Sciences of Sorbonne University, Paris.
I recently revisited this project to clean up the code and add some complementary features.

# Functions and system calls
The only system calls used are putchar() and alloca(). The code supposes that you can define/implement these two functions in your target environment. The code also requires that you have access to the "va_list" type, as well as the va_start(), va_end(), va_arg() macros.
The following functions are implemented:
- vsnprintf()
- vsprintf()
- vprintf()
- snprintf()
- sprintf()
- printf()

Note that these are declared with the prefix "bootmode_" (e.g. bootmode_printf) in print.c and print.h to avoid conflict with the standard C library.
The heart of the project is the bootmode_vsnprintf() function. All of the other functions are essentially wrappers for it.
The functions fprintf and vfprintf are not implemented, as these would require a fputc() and fputs() to be defined in the target environment. Again, if you are able to define a fputc() function in your target environment (and also, therefore, a fputs() function), you can easily implement bootmode_fprintf() and bootmode_vfprintf() as wrappers to bootmode_vsnprintf() function.
The functions asprintf() and vasprintf() are not implemented: through their argument "ret", they return a pointer to dynamically allocated memory containing the formatted string. The problem is that in our context, we cannot use the system call malloc().

# Floating point numbers
Variables or constants of type "float" are converted to type "double" implicitly.
The aforementioned printf-family functions can print both "double" and "long double" arguments (via the 'L' modifier).
Note that "double" is taken to be a IEEE-754 64-bit floating point number, and "long double" is taken to be a IEEE-754 80-bit extended precision floating point number.
As currently implemented, the conversion code supposes that these are padded on the left up to 128 bits, i.e. reading from left to right: 48 padding bits, 1 sign bit, 15 exponent bits, 64 mantissa bits (with the leading mantissa bit being explicit). If this is not the case in your context, you can edit the decomposeDouble() and decomposeLongDouble() functions in conversion.c.
FLoating point conversions are accurate up to the 17th digit after the decimal point in scientific notation.
Precision for floating point conversions is capped at 18 or 19 depending on the format (%e, %f or %g) so as not to show more than 19 significant figures (for instance 18 digits after the decimal point when using %e format: %.500e -> D.DDDDDDDDDDDDDDDDDDe[+/-]XYZ ), as the conversion algorithm used cannot give us more than that.
The conversion algorithm is explained given in the file algorithm.pdf.
WARNING: values of "long doubles" can range up to roughly 10^4932. With the %f conversion format this would mean printing around 5000 characters, and these printf functions provided are not built to handle that. If you may have long double values in excess of 10^300, strongly consider using %e or %g.

# Compilation
The code was written to be compiled by GCC: in particular, it makes use of "// fall through" comments to silence warning from "switch" statement cases that to not include a "break" statement.
The algorithm used to convert floating point numbers uses 128-bit unsigned integers provided by GCC's "__uint128_t" type.

# Ongoing work
Conversion formats i, d, u, x, o, p, n, c, s, e, E, f, F, g, F are implemented.
Length modifiers l, ll, h, hh, z, L are implemented.
Flags #, 0, +, -,  and  ' ' (whitespace) are implemented.
Field width and precision modifiers are implemented.

Formats  a, A  and length modifiers  j, t  are not yet implemented.
Work is ongoing as of 12th of may 2022 (re-engineering of the code + bug checking).
A new version **should** be available in the coming weeks, that adresses some of the limitations, cleans up and refactors the code, Makefile, and the algorithm.pdf explanatory file.


All rights reserved.
