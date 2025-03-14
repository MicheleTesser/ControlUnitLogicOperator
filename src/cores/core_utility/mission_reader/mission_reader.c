#include "mission_reader.h"
#include "mission_locker/mission_locker.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include <stdio.h>
#include <string.h>

struct CarMissionReader_t{
  enum CAR_MISSIONS current_mission;
  struct CanMailbox* p_mailbox_current_mission;
  MissionLockerRead_h o_mission_locker_read;
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

int8_t
car_mission_reader_init(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv conv = {self};
  struct CarMissionReader_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node)
  {
    p_self->p_mailbox_current_mission = 
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_CARMISSION,
          (uint8_t)message_dlc_can2(CAN_ID_CARMISSION));
  }

  if (!p_self->p_mailbox_current_mission)
  {
    return -1;
  }

  if (lock_mission_ref_get(&p_self->o_mission_locker_read)<0)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_current_mission);
    return -2;
  }

  p_self->current_mission = CAR_MISSIONS_NONE;

  return 0;
}

int8_t
car_mission_reader_update(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv conv = {self};
  struct CarMissionReader_t* const p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can2_h_t o2= {0};

  if (!is_mission_locked(&p_self->o_mission_locker_read)
      && hardware_mailbox_read(p_self->p_mailbox_current_mission, &mex))
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, (uint32_t) timer_time_now());
    p_self->current_mission = o2.can_0x047_CarMission.Mission;
  }

  return 0;
}

enum CAR_MISSIONS
car_mission_reader_get_current_mission(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv_const conv = {self};
  const struct CarMissionReader_t* const p_self = conv.clear;

  return  p_self->current_mission;
}

void
car_mission_reader_destroy(CarMissionReader_h* const restrict self)
{
  union CarMissionReader_h_t_conv conv = {self};
  struct CarMissionReader_t* const p_self = conv.clear;

  hardware_free_mailbox_can(&p_self->p_mailbox_current_mission);

}
