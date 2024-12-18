#include "./pumps.h"
#include <stdint.h>

//private

static inline uint8_t pump_init_done(void)
{
    return 0;
}

//public

uint8_t pump_init(void)
{
    if (!pump_init_done()) {
    
    }
    return 0;
}

uint8_t pump_enable(void)
{
    return 0;
}

uint8_t pump_disable(void)
{
    return 0;
}
