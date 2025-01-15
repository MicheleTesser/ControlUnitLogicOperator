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

struct Imu;

int8_t imu_init(void);
const struct Imu* imu_get(void);
struct Imu* imu_get_mut(void);

int8_t imu_calibrate(struct Imu* const restrict self);
int8_t imu_update_info(struct Imu* const restrict self, const enum IMU_Infos info,
        const enum Axis axis, const float value);
float imu_get_info(const struct Imu* const restrict self,
        const enum IMU_Infos info, const enum Axis axis);

void imu_free_read_ptr(void);
void imu_free_mut_ptr(void);

#define IMU_READ_ONLY_ACTION(exp)\
{\
    const struct Imu* const imu_read_ptr = imu_get();\
    exp;\
    imu_free_read_ptr();\
}

#define IMU_MUT_ACTION(exp)\
{\
    struct Imu* const imu_mut_ptr = imu_get_mut();\
    exp;\
    imu_free_mut_ptr();\
}

#endif // !__CAR_IMU__
