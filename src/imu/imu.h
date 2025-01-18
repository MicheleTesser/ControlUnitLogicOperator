#ifndef __CAR_IMU__
#define __CAR_IMU__

#include <stdint.h>

struct Imu_h{
    const uint8_t private_data[1];
};

enum IMU_DATA{
    IMU_ACCELERATION,
};

enum IMU_AXIS{
    AXES_X,
    AXES_Y,
    AXES_Z,
};

int8_t 
imu_init(struct Imu_h* const restrict self __attribute__((__nonnull__)), const uint16_t mailbox);

float
imu_get_data(struct Imu_h* const restrict self __attribute__((__nonnull__)),
        const enum IMU_DATA data_type, const enum IMU_AXIS axes);

#endif // !__CAR_IMU__
