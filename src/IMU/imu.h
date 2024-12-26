#ifndef __CAR_IMU__
#define __CAR_IMU__

#include <stdint.h>

//INFO: leave the incremental value of the enums
enum Axis{
    axis_X=0,
    axis_Y,
    axis_Z,

    NUM_OF_AXIS //INFO: do not use or move this enum
};

//INFO: leave the incremental value of the enums
enum IMU_Infos{
    IMU_accelerations=0,
    IMU_angles,
};

int8_t imu_init(void);
int8_t imu_calibrate(void);
int8_t imu_update_info(const enum IMU_Infos info, const enum Axis axis, const float value);
float imu_get_info(const enum IMU_Infos info, const enum Axis exis);

#endif // !__CAR_IMU__
