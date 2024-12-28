#ifndef __CAR_RTD__
#define __CAR_RTD__

#include <stdint.h>

enum RTD_MODE{
    BUTTON,
    RES,
};

int8_t input_rtd_class_init(void);
int8_t input_rtd_set_mode(const enum RTD_MODE mode);
int8_t input_rtd_check(void);

#endif // !__CAR_RTD__
