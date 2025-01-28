#include "torque_vec_alg.h"
#include "../../../engines/engines.h"
#include "../../../../../../lib/TV/Torque_Vectoring_2024_forsecontiu.h"


void tv_alg_init(void)
{
    Torque_Vectoring_2024_forsecontiu_initialize();
}

void tv_alg_compute(const struct TVInputArgs* const restrict input,
        float o_posTorquesNM[__NUM_OF_ENGINES__])
{
    rtU.ax = input->ax; // m/s^2
    rtU.ay = input->ay; // m/s^2

    rtU.yaw_r = input->yaw_r; // rad/s

    rtU.throttle = input->throttle / 100.0; // 0 to 1
    rtU.steering = input->steering;

    rtU.rpm[0] = input->rpm[0]; // rpm
    rtU.rpm[1] = input->rpm[1]; // rpm
    rtU.rpm[2] = input->rpm[2]; // rpm
    rtU.rpm[3] = input->rpm[3]; // rpm


    Torque_Vectoring_2024_forsecontiu_step();

    o_posTorquesNM[0] =rtY.T_pos[0];
    o_posTorquesNM[1] =rtY.T_pos[1];
    o_posTorquesNM[2] =rtY.T_pos[2];
    o_posTorquesNM[3] =rtY.T_pos[3];
}
