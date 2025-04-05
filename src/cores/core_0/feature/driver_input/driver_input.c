#include "driver_input.h"
#include "../../../core_utility/mission_reader/mission_reader.h"
#include "../../../core_utility/driver_input_reader/driver_input_reader.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"

#include <stdint.h>
#include <string.h>

//private

struct DriverInput_t{
  enum DRIVER current_driver;
  struct{
    GpioRead_h gpio_rtd_button;
    uint8_t dv_rtd_input_request:1;
  }Rtd_request;
  DriverInputReader_h o_driver_input_reader;
  CarMissionReader_h* p_car_mission;
  struct CanMailbox* p_mailbox_recv_dv_mission;
};

union DriverInput_h_t_conv{
  DriverInput_h* const restrict hidden;
  struct DriverInput_t* const restrict clear;
};

union DriverInput_h_t_conv_const{
  const DriverInput_h* const restrict hidden;
  const struct DriverInput_t* const restrict clear;
};

union DriverInputConv{
  DriverInput_h* const hidden;
  struct DriverInput_t* const clear;
};

#ifdef DEBUG
const uint8_t __assert_driver_input_size[sizeof(DriverInput_h) == sizeof(struct DriverInput_t)? 1 : -1];
const uint8_t __assert_driver_input_align[_Alignof(DriverInput_h) == _Alignof(struct DriverInput_t)? 1 : -1];
#endif /* ifdef DEBUG */

//public

int8_t driver_input_init(DriverInput_h* const restrict self, 
    CarMissionReader_h* const restrict p_car_mission)
{
  union DriverInputConv d_conv = {self};
  struct DriverInput_t* const p_self = d_conv.clear;
  struct CanNode* p_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_read_permission_gpio(&p_self->Rtd_request.gpio_rtd_button, GPIO_RTD_BUTTON)<0)
  {
    return -1;
  }

  if (driver_input_reader_init(&p_self->o_driver_input_reader) < 0)
  {
    return -2;
  }

  ACTION_ON_CAN_NODE(CAN_DV, p_node)
  {
    p_self->p_mailbox_recv_dv_mission = hardware_get_mailbox_single_mex(
        p_node,
        RECV_MAILBOX,
        CAN_ID_DV_MISSION,
        message_dlc_can3(CAN_ID_DV_MISSION));
  };

  if (!p_self->p_mailbox_recv_dv_mission)
  {
    return -3;
  }

  p_self->current_driver = DRIVER_NONE;
  p_self->p_car_mission = p_car_mission;
  //TODO: define update rtd request DV

  return 0;
}

int8_t giei_driver_input_get(const DriverInput_h* const restrict self,
    const enum INPUT_TYPES input_type)
{
  const union DriverInput_h_t_conv_const conv = {self};
  const struct DriverInput_t* const p_self = conv.clear;

  switch (p_self->current_driver) {
    case DRIVER_HUMAN:
    case DRIVER_EMBEDDED:
      return driver_input_reader_get(&p_self->o_driver_input_reader, p_self->current_driver, input_type);
    default:
      return -1;
  }

  return 0;
}

int8_t giei_driver_input_rtd_request(const DriverInput_h* const restrict self)
{
  const union DriverInput_h_t_conv_const conv = {self};
  const struct DriverInput_t* const p_self = conv.clear;
  switch (p_self->current_driver) {
    case DRIVER_HUMAN:
      return gpio_read_state(&p_self->Rtd_request.gpio_rtd_button);
    case DRIVER_EMBEDDED:
      return p_self->Rtd_request.dv_rtd_input_request;
    case DRIVER_NONE:
      return 0;
    default:
      return -1;
  }
}

int8_t giei_driver_input_update(DriverInput_h* const restrict self )
{
  union DriverInput_h_t_conv conv = {self};
  struct DriverInput_t* const restrict p_self = conv.clear;
  CanMessage mex = {0};
  can_obj_can3_h_t o3 = {0};

  driver_input_reader_update(&p_self->o_driver_input_reader);

  switch (car_mission_reader_get_current_mission(p_self->p_car_mission))
  {
    case CAR_MISSIONS_NONE:
      p_self->current_driver = DRIVER_NONE;
      break;
    case CAR_MISSIONS_HUMAN:
      p_self->current_driver = DRIVER_HUMAN;
      break;
    case CAR_MISSIONS_DV_SKIDPAD:
    case CAR_MISSIONS_DV_AUTOCROSS:
    case CAR_MISSIONS_DV_TRACKDRIVE:
    case CAR_MISSIONS_DV_EBS_TEST:
    case CAR_MISSIONS_DV_INSPECTION:
      p_self->current_driver = DRIVER_EMBEDDED;
      if (hardware_mailbox_read(p_self->p_mailbox_recv_dv_mission, &mex))
      {
        unpack_message_can3(&o3, mex.id, mex.full_word, mex.message_size, timer_time_now());
        if (o3.can_0x07e_DV_Mission.Mission_status == 2)
        {
          p_self->Rtd_request.dv_rtd_input_request = 0;
        }
      
      }
      break;
    case __NUM_OF_CAR_MISSIONS__:
      break;
  }

  //TODO: define update rtd request DV

  return 0;
}

void driver_input_destroy(DriverInput_h* restrict self)
{
  const union DriverInput_h_t_conv conv = {self};
  struct DriverInput_t* const p_self = conv.clear;

  driver_input_reader_destroy(&p_self->o_driver_input_reader);
  car_mission_reader_destroy(p_self->p_car_mission);

  memset(p_self, 0, sizeof(*p_self));
}
