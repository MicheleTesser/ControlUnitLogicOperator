#ifndef __BMS_HV__
#define __BMS_HV__

#include <stdint.h>
#include "../../../lib/raceup_board/raceup_board.h"

#if ARCH == 64
typedef struct __attribute__((aligned(8))) BmsHv_h{
  const uint8_t private_data[56];
}BmsHv_h;
#elif ARCH == 32
typedef struct __attribute__((aligned(4))) BmsHv_h{
  const uint8_t private_data[56];
}BmsHv_h;
#endif // ARCH == 64

enum BMS_HV_ATTRIBUTE{
  BMS_HV_MIN_VOLTS,
  BMS_HV_MAX_VOLTS,
  BMS_HV_MEAN_VOLTS,

  BMS_HV_MIN_TEMPS,
  BMS_HV_MAX_TEMPS,
  BMS_HV_MEAN_TEMPS,

  BMS_HV_MEAN_SOC, //0-100
  BMS_HV_MEAN_FAN_SPEED, //0-100

  __NUM_OF_BMS_HV_ATTRIBUTES__
};

int8_t
bms_hv_start(struct BmsHv_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
bms_hv_set_attribute(struct BmsHv_h* const restrict self,
    const enum BMS_HV_ATTRIBUTE attribute, const float value)__attribute__((__nonnull__(1)));

int8_t
bms_hv_stop(struct BmsHv_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__BMS_HV__
