#include "./giei_imu.h"
#include "speed_alg/speed_alg.h"
#include <string.h>

struct GieiImu_t{
    float acc[__NUM_OF_AXIS__];
};

union GieiImu_h_t_conv{
    GieiImu_h* const restrict hidden;
    struct GieiImu_t* const restrict clear;
};

union GieiImu_h_t_conv_const{
    const GieiImu_h* const restrict hidden;
    const struct GieiImu_t* const restrict clear;
};

int8_t giei_imu_init(GieiImu_h* const restrict self)
{
    union GieiImu_h_t_conv conv = {self};
    struct GieiImu_t* const restrict p_self = conv.clear;
    memset(p_self, 0, sizeof(*p_self));
    speed_alg_init();

    return 0;
}

int8_t giei_imu_get_acc(const GieiImu_h* const restrict self,
        const enum IMU_AXIS axes)
{
    const union GieiImu_h_t_conv_const conv = {self};
    const struct GieiImu_t* const restrict p_self = conv.clear;
    if (axes != __NUM_OF_AXIS__) {
        return p_self->acc[axes];
    }
    return -1;
}
int8_t giei_imu_get_speed(const GieiImu_h* const restrict self)
{
    const union GieiImu_h_t_conv_const conv = {self};
    const struct GieiImu_t* const restrict p_self __attribute__((__unused__)) = conv.clear;

    return 0;
}
