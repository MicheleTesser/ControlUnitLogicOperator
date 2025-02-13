#include "mission.h"
#include <stdint.h>
#include <string.h>
#include "../../../core_utility/mission_locker/mission_locker.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"

struct DvMission_t{
  enum MISSIONS current_mission;
  enum MISSION_STATUS mission_status;
  MissionLockerRead_h mission_locker;
  struct CanMailbox* mission_mailbox;
  struct CanMailbox* mission_status_mailbox;
};

union DvMission_h_t_conv{
  DvMission_h* const restrict hidden;
  struct DvMission_t* const restrict clear;
};

union DvMission_h_t_conv_const{
  const DvMission_h* const restrict hidden;
  const struct DvMission_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_2_mission[(sizeof(DvMission_h) == sizeof(struct DvMission_t))? 1:-1];
#endif // DEBUG
int8_t

dv_mission_init(DvMission_h* const restrict self )
{
  union DvMission_h_t_conv conv = {.hidden = self};
  struct DvMission_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->current_mission = NONE;
  p_self->mission_status = MISSION_NOT_RUNNING;
  ACTION_ON_CAN_NODE(CAN_GENERAL, can_node,
    p_self->mission_mailbox =
    hardware_get_mailbox_single_mex(can_node, RECV_MAILBOX, CAN_ID_CARMISSION, 1);
  )

  ACTION_ON_CAN_NODE(CAN_DV,can_node,
    p_self->mission_status_mailbox =
      hardware_get_mailbox_single_mex(can_node, RECV_MAILBOX, CAN_ID_DV_MISSION, 1);
  )

  if (lock_mission_ref_get(&p_self->mission_locker)<0)
  {
    return -1;
  }

  return 0;
}

int8_t
dv_mission_update(DvMission_h* const restrict self )
{
  union DvMission_h_t_conv conv = {self};
  struct DvMission_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o2;
  can_obj_can3_h_t o3;
  CanMessage mex;

  if (!is_mission_locked(&p_self->mission_locker) && hardware_mailbox_read(p_self->mission_mailbox, &mex)>=0)
  {
    unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, timer_time_now());
    p_self->current_mission = o2.can_0x067_CarMission.Mission;
  }

  if (hardware_mailbox_read(p_self->mission_status_mailbox, &mex)>=0)
  {
    unpack_message_can3(&o3, mex.id, mex.full_word, mex.message_size, timer_time_now());
    p_self->mission_status = o3.can_0x07e_DV_Mission.Mission_status;
  }

  return 0;
}

enum MISSIONS
dv_mission_get_current(const DvMission_h* const restrict self )
{
  const union DvMission_h_t_conv_const conv = {self};
  const struct DvMission_t* const restrict p_self = conv.clear;
  return p_self->current_mission;
}

enum MISSION_STATUS
dv_mission_get_status(const DvMission_h* const restrict self )
{
  const union DvMission_h_t_conv_const conv = {self};
  const struct DvMission_t* const restrict p_self = conv.clear;
  return p_self->mission_status;
}
