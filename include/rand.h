#ifndef RAND_H
#define RAND_H

int rand(void);
#ifdef __MWERKS__
#define abs(x) __abs(x)  /* compiler intrinsic, as in MSL */
#else
#include <stdlib.h>
#endif

#define RAND_FLOAT() (rand() / 32767.0f)

#endif
