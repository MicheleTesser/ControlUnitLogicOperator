#include "driver_input_reader.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include "../shared_message/shared_message.h"
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
  SharedMessageReader_h m_recv_human_brake;
  SharedMessageReader_h m_recv_dv_driver;
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

  memset(p_self, 0, sizeof(*p_self));

  if (shared_message_reader_init(&p_self->m_recv_human_brake, SHARED_MEX_DRIVER))
  {
    return -1;
  }

  if (shared_message_reader_init(&p_self->m_recv_dv_driver,SHARED_MEX_DV_DRIVER))
  {
    return -2;
  }

  return 0;
}

int8_t driver_input_reader_update(DriverInputReader_h* const restrict self )
{
  union DriverInput_h_t_conv conv = {self};
  struct DriverInputReader_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o2={0};
  can_obj_can3_h_t o3={0};

  if (shared_message_read_unpack_can2(&p_self->m_recv_human_brake, &o2))
  {
    p_self->m_input[DRIVER_HUMAN].m_throttle = _saturate_100(o2.can_0x053_Driver.throttle);
    p_self->m_input[DRIVER_HUMAN].m_brake = _saturate_100(o2.can_0x053_Driver.brake);
    p_self->m_input[DRIVER_HUMAN].m_steering_angle = _saturate_100(o2.can_0x053_Driver.steering);
  }

  if (shared_message_read_unpack_can3(&p_self->m_recv_dv_driver, &o3))
  {
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
