#include "core_status.h"
#include <stdint.h>

static struct{
    enum CORE_STATUS cores[3];
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
