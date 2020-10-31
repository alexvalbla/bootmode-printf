# printf
A small printf function (and variants) using only small system calls (such as putchar(), ... no malloc() for instance) and no floating point operations. This project is intended for use in boot mode, without access to libc functions, or on systems that need to print floating point numbers but have no floating point computation ability for binary-to-decimal conversion.

This was originally a university project I did with my classmate Bijan Rad at the Faculty of Sciences of Sorbonne University, Paris.
I recently revisited this project and am in the process of cleaning up the code.
The file <algorithm.pdf> details the mathematics behind the conversion algorithms (there are in fact 2, the second being an extension of the first).

All rights reserved.
