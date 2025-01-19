#ifndef __CAR_LOG__
#define __CAR_LOG__

#include <stdint.h>

typedef struct Log_h{
    const uint8_t private_data[1];
}Log_h;

int8_t log_init(Log_h* const restrict);

#endif // !__CAR_LOG__
