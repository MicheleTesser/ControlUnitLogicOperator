#ifndef __GIEI_IMU__
#define __GIEI_IMU__

#include <stdint.h>

typedef struct GieiImu_h{
    const uint8_t private_data[1];
}GieiImu_h;

enum IMU_AXIS{
    AXES_X=0,
    AXES_Y,
    AXES_Z,

    __NUM_OF_AXIS__
};

int8_t giei_imu_init(GieiImu_h* const restrict self __attribute__((__nonnull__)));
int8_t giei_imu_get_acc(const GieiImu_h* const restrict self __attribute__((__nonnull__)),
        const enum IMU_AXIS axes);
int8_t giei_imu_get_speed(const GieiImu_h* const restrict self __attribute__((__nonnull__)));

#endif // !__GIEI_IMU__
