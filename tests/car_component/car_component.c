#include "car_component.h"
#include "amk_inverter/amk_inverter.h"
#include "atc/atc.h"
#include "bms_hv/bms_hv.h"
#include "pcu/pcu.h"
#include "steering_wheel/steering_wheel.h"
#include "embedded_system/embedded_system.h"

#include <stdint.h>

int8_t
start_external_boards(ExternalBoards_t* const restrict self)
{
  if (car_amk_inverter_start(&self->amk_inverter)<0)return -1;
  if (pcu_init(&self->pcu)<0)return -2;
  if (atc_start(&self->atc)<0)return -3;
  if (bms_hv_start(&self->bms_hv)<0)return -4;
  if (steering_wheel_start(&self->steering_wheel)<0)return -4;
  if (embedded_system_start(&self->embedded_system)<0) return  -5;;

  return 0;
}

int8_t
stop_external_boards(ExternalBoards_t* const restrict self)
{
  pcu_stop(&self->pcu);
  atc_stop(&self->atc);
  car_amk_inverter_stop(&self->amk_inverter);
  bms_hv_stop(&self->bms_hv);
  steering_wheel_stop(&self->steering_wheel);
  embedded_system_stop(&self->embedded_system);

  return 0;
}
