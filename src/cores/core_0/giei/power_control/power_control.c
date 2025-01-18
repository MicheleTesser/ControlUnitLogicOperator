#include "./power_control.h"
#include "../engines/engine_common.h"
#include "../math_saturated/saturated.h"
#include <stdint.h>

//private

#define STANDARD_SPEED      1000.0f
#define KP_PI               0.1f
#define KI_PI               10.0f
#define T_SAMPLING_ECU      0.010f      //INFO: sample period [s]
#define PI_UPPER_BOUND      175929.2f

static float PIDController(const float pi_error)
{
    float pGain = pi_error * KP_PI;
    float iGain = pi_error * KI_PI * T_SAMPLING_ECU;
    float anti_wind_up =0;

    iGain+= anti_wind_up;

    iGain = saturate_float(iGain, PI_UPPER_BOUND, 0);

    anti_wind_up = iGain;

    float pe_red = saturate_float(iGain + pGain, PI_UPPER_BOUND, 0);

    return pe_red/STANDARD_SPEED;
}

//public
void powerControl(const float total_power, const float power_limit, 
        float posTorquesNM[NUM_OF_EGINES])
{
    float sTorque = 0;
    float power_error =2;
    float reduction_factor=0;
    float unsaturated_reduction = 0;

    for (uint8_t i = 0; i <= REAR_RIGHT; i++){
        sTorque+=posTorquesNM[i];
    }

    power_error = total_power - power_limit; //limited by BMS temp

    unsaturated_reduction = PIDController(power_error);
    reduction_factor = saturate_float(unsaturated_reduction, sTorque*(0.99f), 0)/sTorque;

    if (reduction_factor > 0)
    {
        for (uint8_t i = 0; (i < NUM_OF_EGINES); i++){
            posTorquesNM[i] = posTorquesNM[i] - (reduction_factor*posTorquesNM[i]);
        }
    }
}
