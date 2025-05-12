#include "driver_input_reader.h"
#include "../../../lib/raceup_board/raceup_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#pragma GCC diagnostic pop 

#include <string.h>

struct DriverInputReader_t{
  struct{
    float m_throttle;
    float m_brake;
    float m_steering_angle;
  }m_input[__NUM_OF_DRIVERS__ -1];
  struct CanMailbox* p_human_brake_mailbox;
  struct CanMailbox* p_dv_driver_mailbox;
};

union DriverInput_h_t_conv{
  DriverInputReader_h* const hidden;
  struct DriverInputReader_t* const clear;
};

union DriverInput_h_t_conv_const{
  const DriverInputReader_h* const hidden;
  const struct DriverInputReader_t* const clear;
};

#ifdef DEBUG
char __assert_size_dv_driver_input_reader[(sizeof(DriverInputReader_h) == sizeof(struct DriverInputReader_t))? 1:-1];
char __assert_align_dv_driver_input_reader[(_Alignof(DriverInputReader_h) == _Alignof(struct DriverInputReader_t))? 1:-1];
#endif /* ifdef DEBUG */

static inline float _saturate_100(float value)
{
  return value > 100.0f? 100.0f: value;
}

//public

int8_t driver_input_reader_init(DriverInputReader_h* const restrict self)
{
  union DriverInput_h_t_conv conv ={self};
  struct DriverInputReader_t* const p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->p_human_brake_mailbox=
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_DRIVER,
        message_dlc_can2(CAN_ID_DRIVER));
  }
  ACTION_ON_CAN_NODE(CAN_DV,can_node)
  {
    p_self->p_dv_driver_mailbox =
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_DV_DRIVER,
        message_dlc_can2(CAN_ID_DV_DRIVER));
  }

  return 0;
}

int8_t driver_input_reader_update(DriverInputReader_h* const restrict self )
{
  union DriverInput_h_t_conv conv = {self};
  struct DriverInputReader_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o2={0};
  can_obj_can3_h_t o3={0};
  CanMessage mex={0};

  if (hardware_mailbox_read(p_self->p_human_brake_mailbox, &mex))
  {
    unpack_message_can2(&o2, CAN_ID_DRIVER, mex.full_word, mex.message_size, timer_time_now());

    p_self->m_input[DRIVER_HUMAN].m_throttle = _saturate_100(o2.can_0x053_Driver.throttle);
    p_self->m_input[DRIVER_HUMAN].m_brake = _saturate_100(o2.can_0x053_Driver.brake);
    p_self->m_input[DRIVER_HUMAN].m_steering_angle = _saturate_100(o2.can_0x053_Driver.steering);
  }

  if (hardware_mailbox_read(p_self->p_dv_driver_mailbox, &mex))
  {
    unpack_message_can3(&o3, CAN_ID_DV_DRIVER, mex.full_word, mex.message_size, timer_time_now());

    p_self->m_input[DRIVER_EMBEDDED].m_throttle = _saturate_100(o3.can_0x07d_DV_Driver.Throttle);
    p_self->m_input[DRIVER_EMBEDDED].m_brake = _saturate_100(o3.can_0x07d_DV_Driver.Brake);
    p_self->m_input[DRIVER_EMBEDDED].m_steering_angle = _saturate_100(o3.can_0x07d_DV_Driver.Steering_angle);
  }

  return 0;
}

float driver_input_reader_get(const DriverInputReader_h* const restrict self,
    const enum DRIVER driver_type,
    const enum INPUT_TYPES driver_input)
{
  const union DriverInput_h_t_conv_const conv = {self};
  const struct DriverInputReader_t* const p_self = conv.clear;

  if (driver_type == DRIVER_NONE || driver_type >= __NUM_OF_DRIVERS__)
  {
    return -1;
  }

  switch (driver_input)
  {
    case THROTTLE:
      return p_self->m_input[driver_type].m_throttle;
    case BRAKE:
      return p_self->m_input[driver_type].m_brake;
    case STEERING_ANGLE:
      return p_self->m_input[driver_type].m_steering_angle;
    case __NUM_OF_INPUT_TYPES__:
    default:
      return -1;
  }

  return 0;
}

void driver_input_reader_destroy(DriverInputReader_h* restrict self)
{
  union DriverInput_h_t_conv conv ={self};
  struct DriverInputReader_t* const p_self = conv.clear;

  hardware_free_mailbox_can(&p_self->p_human_brake_mailbox);
  hardware_free_mailbox_can(&p_self->p_dv_driver_mailbox);
}
