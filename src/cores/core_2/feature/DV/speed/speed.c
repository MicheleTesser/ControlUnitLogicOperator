#include "speed.h"
#include "imu/imu.h"
#include <stdint.h>
#include <string.h>


struct DvSpeed_t{
    DvImu_h imu;
};

union DvSpeed_h_t_conv{
    DvSpeed_h* const restrict hidden;
    struct DvSpeed_t* const restrict clear;
};

#ifdef DEBUG
uint8_t __assert_size_dv_speed[(sizeof(DvSpeed_h) == sizeof(struct DvSpeed_t))? 1 : -1];
uint8_t __assert_align_dv_speed[(_Alignof(DvSpeed_h) == _Alignof(struct DvSpeed_t))? 1 : -1];
#endif // DEBUG

int8_t
dv_speed_init(DvSpeed_h* const restrict self )
{
    union DvSpeed_h_t_conv conv = {self};
    struct DvSpeed_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    if(dv_imu_init(&p_self->imu) <0){
      return -1;
    }

    return 0;
}

int8_t
dv_speed_update(DvSpeed_h* const restrict self )
{
    union DvSpeed_h_t_conv conv = {self};
    struct DvSpeed_t* const restrict p_self = conv.clear;
    return dv_imu_update(&p_self->imu);
}

float
dv_speed_get(DvSpeed_h* const restrict self __attribute__((__unused__)))
{
  return 0;
}
