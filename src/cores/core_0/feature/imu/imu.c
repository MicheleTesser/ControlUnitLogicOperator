#include "imu.h"
#include <string.h>

//TODO: imu can messages not yet defined

struct Imu_t{
    float acc[__NUM_OF_AXIS__];
};

union GieiImu_h_t_conv{
    Imu_h* const restrict hidden;
    struct Imu_t* const restrict clear;
};

union GieiImu_h_t_conv_const{
    const Imu_h* const restrict hidden;
    const struct Imu_t* const restrict clear;
};

int8_t giei_imu_init(Imu_h* const restrict self)
{
    union GieiImu_h_t_conv conv = {self};
    struct Imu_t* const restrict p_self = conv.clear;
    memset(p_self, 0, sizeof(*p_self));

    return 0;
}

int8_t giei_imu_get_acc(const Imu_h* const restrict self,
        const enum IMU_AXIS axes)
{
    const union GieiImu_h_t_conv_const conv = {self};
    const struct Imu_t* const restrict p_self = conv.clear;
    if (axes != __NUM_OF_AXIS__) {
        return p_self->acc[axes];
    }
    return -1;
}
int8_t giei_imu_get_speed(const Imu_h* const restrict self)
{
    const union GieiImu_h_t_conv_const conv = {self};
    const struct Imu_t* const restrict p_self __attribute__((__unused__)) = conv.clear;

    return 0;
}
