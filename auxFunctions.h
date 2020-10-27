typedef enum {SPACE_PADDED = 0, ZERO_PADDED, RIGHT_PADDED} P_mode;
//for padding in case of specified field length

typedef enum {NO_SIGN = 0, BLANK, SIGN} S_mode;
//sign flags

void reverse_string(char *s, unsigned int size);

void print_string(char *s);

void string_s(char *s, char *str, unsigned int strIndex, P_mode Padding_F, unsigned int length, unsigned int padding_length);

int string_d(int a, char *s);

int string_ld(long int a, char *s);

int string_lld(long long int a, char *s);

int string_u(unsigned int a, char *s);

int string_lu(long unsigned int a, char *s);

int string_llu(long long unsigned int a, char *s);

int string_x(unsigned int n, char *s, int mod);

int string_lx(long unsigned int n, char *s, int mod);

int string_llx(long long unsigned int n, char *s, int mod);

int string_o(unsigned int n, char *s);

int string_lo(unsigned long int n, char *s);

int string_llo(unsigned long long int n, char *s);

int string_e(double f, char *s, unsigned int decimal);

int string_Le(long double f, char *s, unsigned int decimal);

int string_f(double f, char *s, unsigned int decimal);

int string_Lf(long double f, char *s, unsigned int decimal);

int string_g(double f, char *s, unsigned int decimal);

int string_Lg(long double f, char *s, unsigned int decimal);
