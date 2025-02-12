#ifndef __CAR_AMK_INVERTER__
#define __CAR_AMK_INVERTER__

#include <stdint.h>

enum INVERTER_ATTRIBUTE {
  ERROR,
  WARNING,
  DERATRING,
  SYSTEM_READY,
};

enum ENGINE_STATUS{
  ACTUAL_VELOCITY,
  TORQUE_CURRENT,
  MAGNETIZING_CURRENT,
  TEMP_MOTOR,
  TEMP_INVERTER,
  TEMP_IGBT,
  ERROR_INFO,
};

typedef struct EmulationAmkInverter_h{
  const char private_date[120];
}EmulationAmkInverter_h;

int8_t
car_amk_inverter_class_init(struct EmulationAmkInverter_h* self)__attribute__((__nonnull__(1)));

void 
car_amk_inverter_reset(struct EmulationAmkInverter_h* self)__attribute__((__nonnull__(1)));

int8_t 
car_amk_inverter_set_attribute(struct EmulationAmkInverter_h* self,
    const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
    const int64_t value)__attribute__((__nonnull__(1)));

int8_t
car_amk_inverter_set_engine_value(struct EmulationAmkInverter_h* self,
    const enum ENGINE_STATUS status, const uint8_t engine, 
    const float value)__attribute__((__nonnull__(1)));

void car_amk_inverter_stop(struct EmulationAmkInverter_h* self)__attribute__((__nonnull__(1)));

#endif // !__CAR_AMK_INVERTER__
