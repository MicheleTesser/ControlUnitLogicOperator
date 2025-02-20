#include "car_component.h"
#include "amk_inverter/amk_inverter.h"
#include "atc/atc.h"
#include "bms_hv/bms_hv.h"
#include "pcu/pcu.h"

#include <stdint.h>
#include <stdio.h>

int8_t
start_external_boards(ExternalBoards_t* const restrict self)
{
  if (car_amk_inverter_start(&self->amk_inverter)<0)return -1;
  if (pcu_init(&self->pcu)<0)return -2;
  if (atc_start(&self->atc)<0)return -3;
  if (bms_hv_start(&self->bms_hv)<0)return -4;

  return 0;
}

int8_t
stop_external_boards(ExternalBoards_t* const restrict self)
{
  pcu_stop(&self->pcu);
  atc_stop(&self->atc);
  car_amk_inverter_stop(&self->amk_inverter);
  bms_hv_stop(&self->bms_hv);

  return 0;
}
