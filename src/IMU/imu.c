#include "imu.h"
#include <stdint.h>
#include <time.h>

//private

static const float calibration_vector[3][3] = {
    {0.9989f,-0.0464f,0.0076f}, //INFO: axis X
    {0.0463f,0.9988f,-0.0128f}, //INFO: axis Y
    {0.0082f,0.0125f,0.9999f}   //INFO: axis Z
};

static struct Imu{
    float accelerations[3];
    float omegas[3];
    uint8_t init_done:1;
    uint8_t mut_ptr:1;
    uint8_t read_ptr: 6;
}IMU;

static void calibrate_axis(struct Imu* const restrict self, const enum Axis axis){
    self->accelerations[axis] = 
        self->accelerations[axis_X] * calibration_vector[axis][0] + 
        self->accelerations[axis_Y] * calibration_vector[axis][1] + 
        self->accelerations[axis_Z] * calibration_vector[axis][2];
}

static float* get_buffer_mut(struct Imu* const restrict self, const enum IMU_Infos info, const enum Axis axis)
{
    switch (info) {
        case IMU_accelerations:
            return &self->accelerations[axis];
        case IMU_angles:
            return &self->omegas[axis];
        default:
            return NULL;
    }
}

static const float* get_buffer(const struct Imu* const restrict self, const enum IMU_Infos info, const enum Axis axis)
{
    switch (info) {
        case IMU_accelerations:
            return &self->accelerations[axis];
        case IMU_angles:
            return &self->omegas[axis];
        default:
            return NULL;
    }
}

//public

int8_t imu_init(void)
{
    IMU.init_done =1;
    return 0;
}

const struct Imu* imu_get(void)
{
    while (!IMU.init_done || IMU.mut_ptr) {}
    IMU.read_ptr++;
    return &IMU;
}

struct Imu* imu_get_mut(void)
{
    while (!IMU.init_done || IMU.mut_ptr || IMU.read_ptr) {}
    IMU.mut_ptr++;
    return &IMU;
}

int8_t imu_calibrate(struct Imu* const restrict self)
{
    calibrate_axis(self, axis_X);
    calibrate_axis(self, axis_Y);
    calibrate_axis(self, axis_Z);

    return 0;
}
int8_t imu_update_info(struct Imu* const restrict self, const enum IMU_Infos info, 
        const enum Axis axis, const float value)
{
    float* buffer = get_buffer_mut(self, info, axis);
    if (!buffer) {
        return -1;
    }
    *buffer = value;
    return 0;
}

float imu_get_info(const struct Imu* const restrict self,
        const enum IMU_Infos info, const enum Axis axis)
{
    const float* const buffer = get_buffer(self, info, axis);
    if (!buffer) {
        return -1;
    }
    return *buffer;
}

void imu_free_read_ptr(void)
{
    IMU.read_ptr--;
}

void imu_free_mut_ptr(void)
{
    IMU.mut_ptr--;
}
