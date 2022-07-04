# Overview
A family of printf functions (printf, sprintf, snprintf, vprintf, vsprintf, vsnprintf) using only the system call putchar (no malloc for instance) that can print floating point numbers without using floating point operations.
These functions are intended for use in "boot mode" (or whenever you don't have access to libc functions and need your printf function(s) to be "stand-alone"), or on systems that need to print floating point numbers but have no floating point unit for the binary-to-decimal conversion.
This was originally a university project with my classmate Bijan Rad at the Faculty of Sciences of Sorbonne University, Paris.
I recently revisited this project to clean up the code and add some complementary features.

# Functions, system calls, compilation and extension
The code was written to be compiled by GCC (although other compilers such as Clang would surely also work). In particular, it makes use of "// fall through" comments to silence warning from "switch" statement cases that (intentionnaly) have the "break" statement omitted.

The algorithm used to convert floating point numbers uses 128-bit unsigned integers provided by GCC's "__uint128_t" type.
The code also requires that you have access to the "va_list" type, as well as the va_start(), va_end(), va_arg() macros.
The aforementioned functions are implemented under the names:

- bootmode_printf
- bootmode_sprintf
- bootmode_snprintf
- bootmode_vprintf
- bootmode_vsprintf
- bootmode_vsnprintf

The only system call used is putchar. The code supposes that you can define/implement putchar in your target environment (as bm_putchar), as well as a few other things. Everything required is visible in the file src/print.h.
You might want to add a few macros (such as PRIu64, for example) to file src/print.h.
You might also want to extend the code by adding a fputc function to src/print.h, as well as modifying the functions in src/output_utils.c in order to be able to print to the output stream (FILE *) of your choice.
I kept the code as clean as I could in order to aid comprehension for such endeavours...

The functions fprintf and vfprintf are not implemented, as these would require a fputc() and fputs() to be defined in the target environment. Again, if you are able to define a fputc, you could easily implement bootmode_fprintf and bootmode_vfprintf
The functions asprintf and vasprintf are not implemented: through their argument "ret", they return a pointer to dynamically allocated memory containing the formatted string -> in our context however, we cannot use the system call malloc.

# Floating point numbers
Both "double" and "long double" arguments (via the 'L' length modifier) can be printed.
Note that "double" is taken to mean a IEEE-754 64-bit floating point number, and "long double" is taken to be a IEEE-754 80-bit extended precision floating point number, padded "on the left side" to 128 bits, i.e:
 -> reading a "double" from left to right: 1 sign bit, 11 exponent bits, 52 mantissa bits. See the IEEE-754 norm.
 -> reading a "long double" from left to right: 48 padding bits, 1 sign bit, 15 exponent bits, 64 mantissa bits (the leading mantissa bit being explicit in this case). See the IEEE-754 norm.
There is no use to setting the precision above 18 or 19 depending on the format (%e, %E, %f, %F, %g or %G) so as not to show more than 19 significant digits (for instance 18 digits after the decimal point when using %e format: e.g. converting a (double) argument with the formating string "%.500e" -> (-)D.DDDDDDDDDDDDDDDDDDe[+/-]XYZ, where each 'D' represents a decimal mantissa digit, and 'XYZ' represents the decimal exponent).
This is because the conversion algorithm (explained in algorithm.pdf), in the best of cases, cannot give us more than 19 valid decimal digits.
The conversion algorithm has a (small) margin of error: a relative error of around 10^(-18)

# Ongoing work
These "printf-family" functions are written to the specification detailed in https://man7.org/linux/man-pages/man3/printf.3.html, barring a few exceptions (some of these missing features may be added in the near future):

- the * syntax for extracting an (int) argument as field width or precision **is** implemented, but the '*m$' syntax is not.
- the ' (apostrophe) and 'I'   flags are not implemented.
- the formats floating point specifiers   'a', 'A'   are not implemented.
- the length modifier 'l' for %c indicating a (wint_t) argument and for %s indicating a (wchar_t *) argument are not implemented.
- the 'm' specifier for printing output of strerror(errno) is not implemented.

The following options/extensions are either non-standard, obsolete, deprecated, or not recommended, etc. and so are intentionnaly not implemented:

- 'q' as synonym for 'll'
- 'Z' as synonym for 'z'
- 'L' as synonym for 'll' for integer conversions
- 'll' as synonym for 'L' for "long double" conversions
- 'C' as synonym for 'lc'
- 'S' as synonym for 'ls'

To keep the code as simple as possible, a formatting string that cannot be parsed (incorrectly formatted, unknown conversion specifier, etc.) simply results in the printing function in question returning an error (a negative integer). Please be careful with the format string...

Work is ongoing as of the June 18th 2022 (rewriting/re-factoring/re-engineering of the code and algorithm.pdf explanatory file, implementation of some of the missing features mentioned above, and of course bug-hunting :)


All rights reserved.
