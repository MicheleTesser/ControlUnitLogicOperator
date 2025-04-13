#ifndef __GIEI_IMU__
#define __GIEI_IMU__

#include <stdint.h>
#include "../../../lib/raceup_board/raceup_board.h"

#if ARCH == 64
typedef struct __attribute__((aligned(8))){
    const uint8_t private_data[24];
}Imu_h;
#elif ARCH == 32
typedef struct __attribute__((aligned(4))){
    const uint8_t private_data[16];
}Imu_h;
#else
#endif


enum IMU_AXIS{
    AXES_X=0,
    AXES_Y,
    AXES_Z,

    __NUM_OF_AXIS__
};

int8_t imu_init(Imu_h* const restrict self )__attribute__((__nonnull__(1)));
int8_t imu_update(Imu_h* const restrict self )__attribute__((__nonnull__(1)));
float imu_get_acc(const Imu_h* const restrict self ,
        const enum IMU_AXIS axes)__attribute__((__nonnull__(1)));
float imu_get_speed(const Imu_h* const restrict self )__attribute__((__nonnull__(1)));

#endif // !__GIEI_IMU__
