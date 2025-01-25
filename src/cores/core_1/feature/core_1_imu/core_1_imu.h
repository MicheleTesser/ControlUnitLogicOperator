#ifndef __CORE_1_IMU__
#define __CORE_1_IMU__

#include <stdint.h>
#include "../log/log.h"

typedef struct Core1Imu_h{
    const uint8_t private_data[1];
}Core1Imu_h;

int8_t
core_1_imu_init(
        Core1Imu_h* const restrict self ,
        Log_h* const restrict log )__attribute__((__nonnull__(1,2)));

int8_t
core_1_imu_update(Core1Imu_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__CORE_1_IMU__
