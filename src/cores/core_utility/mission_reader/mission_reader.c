#include "mission_reader.h"
#include "mission_locker/mission_locker.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include "../shared_message/shared_message.h"
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include <string.h>

struct CarMissionReader_t{
  enum CAR_MISSIONS m_current_mission;
  time_var_microseconds m_embeed_last_alive;
  SharedMessageReader_h m_recv_current_mission;
  SharedMessageReader_h m_recv_embedded_alive;
  MissionLockerRead_h m_mission_locker_read;
};

union CarMissionReader_h_t_conv{
  CarMissionReader_h* const hidden;
  struct CarMissionReader_t* const clear;
};

union CarMissionReader_h_t_conv_const{
  const CarMissionReader_h* const hidden;
  const struct CarMissionReader_t* const clear;
};


#ifdef DEBUG
char __assert_size_car_mission_reader[(sizeof(CarMissionReader_h)==sizeof(struct CarMissionReader_t))?+1:-1];
char __assert_alignment_car_mission_reader[(_Alignof(CarMissionReader_h)==_Alignof(struct CarMissionReader_t))?+1:-1];
#endif /* ifdef DEBUG */

//public

int8_t car_mission_reader_init(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv conv = {self};
  struct CarMissionReader_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (shared_message_reader_init(&p_self->m_recv_current_mission, SHARED_MEX_CARMISSION))
  {
    return -1;
  }

  if (shared_message_reader_init(&p_self->m_recv_embedded_alive, SHARED_MEX_EMBEDDEDALIVECHECK))
  {
    return -2;
  }

  if (lock_mission_ref_get(&p_self->m_mission_locker_read)<0)
  {
    return -3;
  }

  p_self->m_current_mission = CAR_MISSIONS_NONE;

  return 0;
}

int8_t car_mission_reader_update(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv conv = {self};
  struct CarMissionReader_t* const p_self = conv.clear;
  uint64_t mex = {0};
  can_obj_can2_h_t o2= {0};

  if (shared_message_read(&p_self->m_recv_embedded_alive, &mex))
  {
    p_self->m_embeed_last_alive = timer_time_now(); 
  }

  if ((timer_time_now() - p_self->m_embeed_last_alive) > get_tick_from_millis(500) && 
      p_self->m_current_mission > CAR_MISSIONS_HUMAN &&
      !is_mission_locked(&p_self->m_mission_locker_read))
  {
    p_self->m_current_mission = CAR_MISSIONS_NONE;
  }

  if (!is_mission_locked(&p_self->m_mission_locker_read)
      && shared_message_read_unpack_can2(&p_self->m_recv_current_mission, &o2))
  {
    p_self->m_current_mission = o2.can_0x047_CarMission.Mission;
  }

  return 0;
}

enum CAR_MISSIONS car_mission_reader_get_current_mission(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv_const conv = {self};
  const struct CarMissionReader_t* const p_self = conv.clear;

  return  p_self->m_current_mission;
}

