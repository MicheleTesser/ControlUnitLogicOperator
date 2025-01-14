#include "core_status.h"
#include <stdint.h>

#define NUM_OF_CORES 3

static struct{
    enum CORE_STATUS cores[NUM_OF_CORES];
}CORES;

enum CORE_STATUS core_status(const uint8_t core)
{
    switch (core) {
        case 0:
        case 1:
        case 2:
            return CORES.cores[core];
        default:
            return CORE_ERROR;
    }
}

int8_t core_update_status(const uint8_t core, const enum CORE_STATUS status)
{
    switch (core) {
        case 0:
        case 1:
        case 2:
            CORES.cores[core]= status;
            return 0;
        default:
            return -1;
    }
}

uint8_t are_cores_in_sync(void)
{
    uint8_t res = 1;
    for (uint8_t i =0; i<NUM_OF_CORES; i++) {
        res &= (CORES.cores[i] == CORE_READY);
    }

    return res;
}
