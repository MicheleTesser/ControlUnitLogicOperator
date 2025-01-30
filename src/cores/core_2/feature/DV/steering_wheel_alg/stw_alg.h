#ifndef __DV_STW_ALG__
#define __DV_STW_ALG__


#include <stdint.h>

int8_t
dv_stw_alg_init(void);

int8_t
dv_stw_alg_compute(
    const void* const restrict input,
    void* const restrict output)__attribute__((__nonnull__(1,2)));

#endif // !__DV_STW_ALG__
