#ifndef __GIEI_IMU__
#define __GIEI_IMU__

#include <stdint.h>

typedef struct Imu_h{
    const uint8_t private_data[1];
}Imu_h;

enum IMU_AXIS{
    AXES_X=0,
    AXES_Y,
    AXES_Z,

    __NUM_OF_AXIS__
};

int8_t imu_init(Imu_h* const restrict self __attribute__((__nonnull__)));
int8_t imu_update(Imu_h* const restrict self __attribute__((__nonnull__)));
int8_t imu_get_acc(const Imu_h* const restrict self __attribute__((__nonnull__)),
        const enum IMU_AXIS axes);
int8_t imu_get_speed(const Imu_h* const restrict self __attribute__((__nonnull__)));

#endif // !__GIEI_IMU__
