#include "./regen_alg.h"
#include "../../../../../../lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22.h"

void regen_alg_init(void)
{
    rigen_fun_simulink22_initialize();
}

void regen_alg_compute
    (const struct RegenAlgInput* const restrict input,float o_negTorquesNM[__NUM_OF_ENGINES__])
{
    rigen_fun_simulink22_U.voltage = input->battery_pack_tension;
    rigen_fun_simulink22_U.rpmFL = input->front_left_velocity;
    rigen_fun_simulink22_U.rpmFR = input->front_right_velocity;
    rigen_fun_simulink22_U.rpmRL = input->rear_left_velocity;
    rigen_fun_simulink22_U.rpmRR = input->rear_right_velocity;

    rigen_fun_simulink22_step();

    o_negTorquesNM[REAR_LEFT] = (float)rigen_fun_simulink22_Y.TorqueRL;
    o_negTorquesNM[REAR_RIGHT] = (float)rigen_fun_simulink22_Y.TorqueRR;
    o_negTorquesNM[FRONT_LEFT] = (float)rigen_fun_simulink22_Y.TorqueFL;
    o_negTorquesNM[FRONT_RIGHT] = (float)rigen_fun_simulink22_Y.TorqueFR;
}
