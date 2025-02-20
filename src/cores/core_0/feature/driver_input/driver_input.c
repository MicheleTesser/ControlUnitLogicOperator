#include "driver_input.h"
#include "../../../core_utility/mission_reader/mission_reader.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

//private

typedef int16_t index_tye;
struct DriverInput_t{
  float driver_data[(__NUM_OF_DRIVERS__ - 1)  * __NUM_OF_INPUT_TYPES__];
  enum DRIVER current_driver;
  GpioRead_h gpio_rtd_button;
  uint8_t dv_rtd_input_request:1;
  struct CanMailbox* p_drivers_mailboxes[__NUM_OF_DRIVERS__ -1];
  CarMissionReader_h* p_car_mission;
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
  struct DriverInput_h* const hidden;
  struct DriverInput_t* const clear;
};

#ifdef DEBUG
const uint8_t __assert_driver_input_size[sizeof(DriverInput_h) == sizeof(struct DriverInput_t)? 1 : -1];
const uint8_t __assert_driver_input_align[_Alignof(DriverInput_h) == _Alignof(struct DriverInput_t)? 1 : -1];
#endif /* ifdef DEBUG */

static inline index_tye
compute_data_index(const enum DRIVER driver, const enum INPUT_TYPES driver_input)
{
  if (driver < __NUM_OF_DRIVERS__ && driver != DRIVER_NONE && driver_input < __NUM_OF_INPUT_TYPES__)
  {
    return (driver * (__NUM_OF_DRIVERS__ - 1)) + driver_input;   
  }
  return -1;
}

//public

int8_t
driver_input_init(struct DriverInput_h* const restrict self, 
    CarMissionReader_h* const restrict p_car_mission)
{
  union Conv{
    struct DriverInput_h* const restrict hidden;
    struct DriverInput_t* const restrict clear;
  };
  union Conv d_conv = {self};
  struct DriverInput_t* const p_self = d_conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_read_permission_gpio(&p_self->gpio_rtd_button, GPIO_RTD_BUTTON)<0)
  {
    return -1;
  }

  p_self->current_driver = DRIVER_NONE;

  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->p_drivers_mailboxes[DRIVER_HUMAN] =
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_DRIVER,
        message_dlc_can2(CAN_ID_DRIVER));
  }
  ACTION_ON_CAN_NODE(CAN_DV,can_node)
  {
    p_self->p_drivers_mailboxes[DRIVER_EMBEDDED] =
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        CAN_ID_DV_DRIVER,
        message_dlc_can2(CAN_ID_DV_DRIVER));
  }

  p_self->p_car_mission = p_car_mission;

  return 0;
}

int8_t
driver_input_update(DriverInput_h* const restrict self )
{
  union DriverInput_h_t_conv conv = {self};
  struct DriverInput_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o2={0};
  can_obj_can3_h_t o3={0};
  CanMessage mex={0};
  struct CanMailbox* mailbox = NULL;
  index_tye index_input = 0;

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
      break;
    case __NUM_OF_CAR_MISSIONS__:
      break;
  }

  switch (p_self->current_driver)
  {
    case DRIVER_HUMAN:
      mailbox = p_self->p_drivers_mailboxes[DRIVER_HUMAN];
      if (!hardware_mailbox_read(mailbox, &mex))
      {
        unpack_message_can2(&o2, CAN_ID_DRIVER, mex.full_word, mex.message_size, timer_time_now());
        index_input = compute_data_index(DRIVER_HUMAN, THROTTLE);
        if (index_input<0)
        {
          return -1;
        }
        p_self->driver_data[index_input] = o2.can_0x053_Driver.throttle;

        index_input = compute_data_index(DRIVER_HUMAN, BRAKE);
        if (index_input<0)
        {
          return -1;
        }
        p_self->driver_data[index_input] = o2.can_0x053_Driver.brake;

        index_input = compute_data_index(DRIVER_HUMAN, STEERING_ANGLE);
        if (index_input<0)
        {
          return -1;
        }
        p_self->driver_data[index_input] = o2.can_0x053_Driver.steering;
      }
      break;
    case DRIVER_EMBEDDED:
      mailbox = p_self->p_drivers_mailboxes[DRIVER_EMBEDDED];
      if (hardware_mailbox_read(mailbox, &mex)>=0) {
        unpack_message_can3(&o3, CAN_ID_DV_DRIVER, mex.full_word, mex.message_size, timer_time_now());
        index_input = compute_data_index(DRIVER_EMBEDDED, THROTTLE);
        if (index_input<0)
        {
          return -1;
        }
        p_self->driver_data[index_input] = o3.can_0x07d_DV_Driver.Throttle;
        index_input = compute_data_index(DRIVER_EMBEDDED, BRAKE);
        if (index_input<0)
        {
          return -1;
        }
        p_self->driver_data[index_input] = o3.can_0x07d_DV_Driver.Brake;
        index_input = compute_data_index(DRIVER_EMBEDDED, STEERING_ANGLE);
        if (index_input<0)
        {
          return -1;
        }
        p_self->driver_data[index_input] = o3.can_0x07d_DV_Driver.Steering_angle;

        //TODO: add message for dv rtd request
      }
      break;
    default:
      return -1; 
  }
  return 0;
}

float
driver_input_get(const struct DriverInput_h* const restrict self, 
    const enum INPUT_TYPES driver_input)
{
  const union DriverInput_h_t_conv_const conv = {self};
  const struct DriverInput_t* const p_self = conv.clear;
  const index_tye data_index = compute_data_index(p_self->current_driver, driver_input);

  if (data_index != -1)
  {
    return p_self->driver_data[data_index];
  }
  return -1;
}

int8_t
driver_input_rtd_request(const DriverInput_h* const restrict self)
{
  const union DriverInput_h_t_conv_const conv = {self};
  const struct DriverInput_t* const p_self = conv.clear;
  switch (p_self->current_driver) {
    case DRIVER_HUMAN:
      return gpio_read_state(&p_self->gpio_rtd_button);
    case DRIVER_EMBEDDED:
      return p_self->dv_rtd_input_request;
    case DRIVER_NONE:
      return 0;
    default:
      return -1;
  }
}

void
driver_input_destroy(struct DriverInput_h* restrict self)
{
  union Conv{
    struct DriverInput_h* const restrict hidden;
    struct DriverInput_t* const restrict clear;
  };
  union Conv d_conv = {self};
  struct DriverInput_t* const p_self = d_conv.clear;
  for (uint8_t i=0; i < __NUM_OF_DRIVERS__; i++ )
  {
    hardware_free_mailbox_can(&p_self->p_drivers_mailboxes[i]);
  }
}
