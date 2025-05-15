#include "core_status.h"
#include <stdint.h>

static uint8_t READY_COUNT[__NUM_OF_CORES__]={0};

void core_status_core_ready(const enum CORES core)
{
    if (core < __NUM_OF_CORES__)
    {
        READY_COUNT[core] = 1;
    }
}

uint8_t core_status_ready_state(void)
{
    uint8_t res = 1;
    for (uint8_t i =0; i<__NUM_OF_CORES__; i++)
    {
        res &= READY_COUNT[i];
    }
    return res;
}
