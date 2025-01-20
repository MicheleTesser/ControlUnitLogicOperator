#include "running_status.h"
#include <stdatomic.h>

static atomic_uchar global_car_status = SYSTEM_OFF;

int8_t global_running_status_set(const enum RUNNING_STATUS status)
{
    atomic_store(&global_car_status, status);
    return 0;
}
enum RUNNING_STATUS global_running_status_get(void)
{
    return atomic_load(&global_car_status);
}
