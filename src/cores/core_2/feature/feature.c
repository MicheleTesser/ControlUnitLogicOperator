#include "feature.h"

#include "DV/dv.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include "../../core_utility/core_utility.h"

#include <stdint.h>

struct Core2Feature_t{
  CarMissionReader_h m_mission;
  Dv_h m_dv;
};

union Core2Feature_h_t_conv{
  Core2Feature_h* const restrict hidden;
  struct Core2Feature_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_2_feature[(sizeof(Core2Feature_h) == sizeof(struct Core2Feature_t))? 1:-1];
char __assert_align_core_2_feature[(_Alignof(Core2Feature_h) == _Alignof(struct Core2Feature_t))? 1:-1];
#endif // DEBUG

int8_t core_2_feature_init(Core2Feature_h* const restrict self )
{
  union Core2Feature_h_t_conv conv = {self};
  struct Core2Feature_t* const restrict p_self = conv.clear;

  if(car_mission_reader_init(&p_self->m_mission)<0) return -1;
  if(dv_class_init(&p_self->m_dv, &p_self->m_mission) <0) return -2;

  return 0;
}

#define ERR_TRACE()\
  {\
    SET_TRACE(CORE_2);\
    err--;\
  }

int8_t core_2_feature_update(Core2Feature_h* const restrict self )
{
  union Core2Feature_h_t_conv conv = {self};
  struct Core2Feature_t* const restrict p_self = conv.clear;
  int8_t err=0;

  if(car_mission_reader_update(&p_self->m_mission) <0) ERR_TRACE();
  if(dv_update(&p_self->m_dv)) ERR_TRACE();

  return err;
}
