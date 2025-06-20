#ifndef __CAR_AMK_INVERTER__
#define __CAR_AMK_INVERTER__

#include <stdint.h>

enum INVERTER_ATTRIBUTE {
  ERROR=0,
  WARNING,
  DERATRING,
  SYSTEM_READY,
};

enum ENGINE_STATUS{
  ACTUAL_VELOCITY=0,
  TORQUE_CURRENT,
  MAGNETIZING_CURRENT,
  TEMP_MOTOR,
  TEMP_INVERTER,
  TEMP_IGBT,
  ERROR_INFO,
  ENGINES_RPM,
};

enum PRECHARGE_STATUS{
  PRECHARGE_OFF=0,
  PRECHARGE_FINISHED,
};

enum INVERTER_STATUS_INFO{
  AMK_INVERTER_TARGET_VELOCITY=0,
};

typedef struct __attribute__((aligned(4))){
  const char private_date[120];
}EmulationAmkInverter_h;

int8_t
car_amk_inverter_start(EmulationAmkInverter_h* self)__attribute__((__nonnull__(1)));

void 
car_amk_inverter_reset(EmulationAmkInverter_h* self)__attribute__((__nonnull__(1)));

int8_t 
car_amk_inverter_set_attribute(EmulationAmkInverter_h* self,
    const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
    const int64_t value)__attribute__((__nonnull__(1)));

int8_t
car_amk_inverter_set_engine_value(EmulationAmkInverter_h* self,
    const enum ENGINE_STATUS status, const uint8_t engine, 
    const float value)__attribute__((__nonnull__(1)));

enum PRECHARGE_STATUS
car_amk_inverter_precharge_status(const EmulationAmkInverter_h* const restrict self)
  __attribute__((__nonnull__(1)));

void
car_amk_inverter_force_precharge_status(EmulationAmkInverter_h* const restrict self)
  __attribute__((__nonnull__(1)));

void
car_amk_inverter_stop(EmulationAmkInverter_h* self)__attribute__((__nonnull__(1)));

int32_t
car_amk_inverter_get_status(EmulationAmkInverter_h* const restrict self,
    const uint8_t engine, const enum INVERTER_STATUS_INFO info)__attribute__((__nonnull__(1)));

#endif // !__CAR_AMK_INVERTER__
