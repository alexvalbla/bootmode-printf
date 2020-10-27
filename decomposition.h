typedef union{
  float f;
  uint32_t i;
} fltcst_t;

typedef union{
  double f;
  uint64_t i;
} dblcst_t;

typedef union{
  long double f;
  __uint128_t i;
} longdblcst_t;

typedef enum {NUMBER = 0, NAN, POS_INF, NEG_INF} fpclass_t;

fpclass_t decomposeFloat(int *s, int32_t *E, uint64_t *m, float x);

fpclass_t decomposeDouble(int *s, int32_t *E, uint64_t *m, double x);

fpclass_t decomposeLongDouble(int *s, int32_t *E, uint64_t *m, long double x);
