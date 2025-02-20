#include "feature.h"

#include "DV/dv.h"
#include "../../../lib/raceup_board/components/can.h"
#include "../../core_utility/mission_reader/mission_reader.h"
#include "dv_driver_input/dv_driver_input.h"

#include <stdint.h>

struct Core2Feature_t{
  DvDriverInput_h driver;
  CarMissionReader_h mission;
  Dv_h dv;
};

union Core2Feature_h_t_conv{
  Core2Feature_h* const restrict hidden;
  struct Core2Feature_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_2_feature[(sizeof(Core2Feature_h) == sizeof(struct Core2Feature_t))? 1:-1];
char __assert_align_core_2_feature[(_Alignof(Core2Feature_h) == _Alignof(struct Core2Feature_t))? 1:-1];
#endif // DEBUG

  int8_t
core_2_feature_init(Core2Feature_h* const restrict self )
{
  union Core2Feature_h_t_conv conv = {self};
  struct Core2Feature_t* const restrict p_self = conv.clear;

  if(dv_driver_input_init(&p_self->driver)<0) return -1;
  if(car_mission_reader_init(&p_self->mission)<0) return -1;
  if(dv_class_init(&p_self->dv, &p_self->mission, &p_self->driver) <0) return -1;

  return 0;
}

  int8_t
core_2_feature_update(Core2Feature_h* const restrict self )
{
  union Core2Feature_h_t_conv conv = {self};
  struct Core2Feature_t* const restrict p_self = conv.clear;

  if(car_mission_reader_update(&p_self->mission) <0) return -1;
  if(dv_driver_input_update(&p_self->driver)) return -1;
  if(dv_update(&p_self->dv)) return -1;

  return 0;
}
