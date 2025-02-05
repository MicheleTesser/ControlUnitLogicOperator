#ifndef __CAR_AMK_INVERTER__
#define __CAR_AMK_INVERTER__

#include <stdint.h>

enum INVERTER_ATTRIBUTE {
  ERROR,
  WARNING,
  DERATRING,
  HV,
  HV_ACK,
  INVERTER_ON,
  INVERTER_ON_ACK,
  SYSTEM_READY,
};

struct AMK_Actual_Values_1{
  struct{
    uint8_t b_reserve; //INFO: Reserved
    uint8_t bSystemReady: 1; //INFO: System ready (SBM)
    uint8_t AMK_bError :1; //INFO: Error
    uint8_t AMK_bWarn :1; //INFO: Warning
    uint8_t AMK_bQuitDcOn :1; //INFO: HV activation acknowledgment
    uint8_t AMK_bDcOn :1; //INFO: HV activation level
    uint8_t AMK_bQuitInverterOn:1; //INFO: Controller enable acknowledgment
    uint8_t AMK_bInverterOn:1; //INFO: Controller enable level
    uint8_t AMK_bDerating :1; //INFO: Derating (torque limitation active)
  }AMK_STATUS;
  //INFO:  AMK_ActualVelocity : Unit = rpm; Actual speed value
  int16_t AMK_ActualVelocity; 
  //INFO: AMK_TorqueCurrent: Raw data for calculating 'actual torque current' Iq See 'Units' on page 65.               
  int16_t AMK_TorqueCurrent; 
  //INFO: Raw data for calculating 'actual magnetizing current' Id See 'Units' on page 1.
  int16_t AMK_MagnetizingCurrent;
};

struct AMK_Actual_Values_2{
  int16_t AMK_TempMotor; //INFO: Unit= 0.1 °C. Motor temperature
  int16_t AMK_TempInverter; //INFO: Unit= 0.1 °C. Cold plate temperature
  uint16_t AMK_ErrorInfo; //INFO: Diagnostic number
  int16_t AMK_TempIGBT; //INFO: 0.1 °C. IGBT temperature
};

struct AMK_Setpoints{
  //INFO: AMK_Control: 
  //Control word See the table below: Content of the 'AMK_Control' control word
  struct {
    uint8_t AMK_bReserve_start;
    uint8_t AMK_bInverterOn:1;
    uint8_t AMK_bDcOn:1;
    uint8_t AMK_bEnable:1;
    uint8_t AMK_bErrorReset:1;
    uint8_t AMK_bReserve_end :4;
  }AMK_Control_fields;
  int16_t AMK_TargetVelocity; //INFO: unit: rpm Speed setpoint
  int16_t AMK_TorqueLimitPositive; //INFO: unit: 0.1% MN Positive torque limit (subject to nominal torque)
  int16_t AMK_TorqueLimitNegative; //INFO: unit: 0.1% MN Negative torque limit (subject to nominal torque)
};

struct AmkInverter{
  struct amk_engines{
    struct AMK_Actual_Values_1 amk_data_1;
    struct AMK_Actual_Values_2 amk_data_2;
  }engines[4];
};

void 
car_amk_inverter_class_init(struct AmkInverter* self);
int8_t 
car_amk_inverter_set_attribute(struct AmkInverter* self,
    const enum INVERTER_ATTRIBUTE attribute, const uint8_t engine,
    const int64_t value)__attribute__((__nonnull__(1)));

#endif // !__CAR_AMK_INVERTER__
