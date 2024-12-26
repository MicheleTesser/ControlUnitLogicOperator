#include "imu.h"
#include <time.h>

//private

static const float calibration_vector[3][3] = {
    {0.9989f,-0.0464f,0.0076f}, //INFO: axis X
    {0.0463f,0.9988f,-0.0128f}, //INFO: axis Y
    {0.0082f,0.0125f,0.9999f}   //INFO: axis Z
};

static struct {
    float accelerations[3];
    float omegas[3];
}IMU;

static void calibrate_axis(const enum Axis axis){
    IMU.accelerations[axis] = 
        IMU.accelerations[axis_X] * calibration_vector[axis][0] + 
        IMU.accelerations[axis_Y] * calibration_vector[axis][1] + 
        IMU.accelerations[axis_Z] * calibration_vector[axis][2];
}

static float* get_buffer(const enum IMU_Infos info, const enum Axis axis)
{
    switch (info) {
        case IMU_accelerations:
            return &IMU.accelerations[axis];
        case IMU_angles:
            return &IMU.omegas[axis];
        default:
            return NULL;
    }
}

//public

int8_t imu_init(void)
{
    return 0;
}

int8_t imu_calibrate(void)
{
    calibrate_axis(axis_X);
    calibrate_axis(axis_Y);
    calibrate_axis(axis_Z);

    return 0;
}
int8_t imu_update_info(const enum IMU_Infos info, const enum Axis axis, const float value)
{
    float* buffer = get_buffer(info, axis);
    if (!buffer) {
        return -1;
    }
    *buffer = value;
    return 0;
}

float imu_get_info(const enum IMU_Infos info, const enum Axis axis)
{
    float* buffer = get_buffer(info, axis);
    if (!buffer) {
        return -1;
    }
    return *buffer;
}
