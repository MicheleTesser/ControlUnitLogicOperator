#ifndef __DEBUG_CAR_COMPONENT__
#define __DEBUG_CAR_COMPONENT__

#include "./amk_inverter/amk_inverter.h"
#include "pcu/pcu.h"
#include "atc/atc.h"
#include "bms_hv/bms_hv.h"

#include <stdint.h>

typedef struct ExternalBoards_t{
  BmsHv_h bms_hv;
  Pcu_h pcu;
  Atc_h atc;
  EmulationAmkInverter_h amk_inverter;
}ExternalBoards_t;

int8_t
start_external_boards(ExternalBoards_t* const restrict self)__attribute__((__nonnull__(1)));

int8_t
stop_external_boards(ExternalBoards_t* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DEBUG_CAR_COMPONENT__
