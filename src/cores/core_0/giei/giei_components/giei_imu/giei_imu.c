#include "./giei_imu.h"
#include "speed_alg/speed_alg.h"

int8_t giei_imu_init(GieiImu_h* const restrict self __attribute__((__nonnull__)));
int8_t giei_imu_get_acc(GieiImu_h* const restrict self __attribute__((__nonnull__)),
        const enum IMU_AXIS axes);
int8_t giei_imu_get_speed(GieiImu_h* const restrict self __attribute__((__nonnull__)));
