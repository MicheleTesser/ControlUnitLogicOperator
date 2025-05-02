#ifndef __IMU__
#define __IMU__

#include <stdint.h>

typedef enum{
  AXES_X=0u,
  AXES_Y,
  AXES_Z,

  __NUM_OF_AXYS__
}ImuAxys;

typedef enum{
  IMU_ACCELERATION=0u,
  IMU_OMEGA,

  __NUM_OF_IMU_PARAMETERS
}ImuPharameter;

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[36];
}imu_h;

int8_t
imu_start(imu_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
imu_set_parameter(imu_h* const restrict self, const ImuPharameter param,
    const ImuAxys axes, const int32_t value)
__attribute__((__nonnull__(1)));

int8_t
imu_stop(imu_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__IMU__
