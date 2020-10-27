static inline int64_t decimal_exponent(int32_t E);

static inline int32_t Delta(int64_t F);

static inline int64_t Sigma(int32_t E, int64_t F, int32_t delta);

void mutiplyM(int64_t F, uint64_t m,  __uint128_t *nHigh, __uint128_t *nMid, __uint128_t *nLow);

__uint128_t decimalMantissa(int64_t F, uint64_t m, int32_t E);

void conversion(int64_t *F, __uint128_t *n, int32_t E, uint64_t m);

int Tau(int64_t F, int Fh, int Fl);

void optimalT(int64_t F, uint64_t *tHigh, uint64_t *tLow);
