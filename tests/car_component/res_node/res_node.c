#include "res_node.h"
#include "../../linux_board/linux_board.h"
#include <sys/cdefs.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../src/lib/board_dbc/dbc/out_lib/can3/can3.h"
#pragma GCC diagnostic pop 
#include "../external_gpio.h"

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

enum RES_STATE {
  RES_OFF,
  RES_NMT,
  RES_OPERATING
};

struct res_node_t{
  uint8_t running;
  thrd_t thread;
  GpioRead_h m_gpio_pcu_dv;
  Gpio_h m_gpio_sdc;
  struct CanNode* p_can_node_dv;
  struct CanMailbox* p_mailbox_send_NMT;
  struct CanMailbox* p_mailbox_send_PDO;
  struct CanMailbox* p_mailbox_operational;
  enum RES_STATE res_state;
  uint8_t remote_running:1;
  uint8_t radio_strenght:7;
  uint8_t lever:1;
  uint8_t button:1;
  uint8_t estop:1;
};

union res_node_h_t_conv{
  res_node_h* const hidden;
  struct res_node_t* const clear;
};

union res_node_h_t_conv_const{
  const res_node_h* const hidden;
  const struct res_node_t* const clear;
};

#ifdef DEBUG
char __assert_size_res_node[(sizeof(res_node_h)==sizeof(struct res_node_t))?+1:-1];
char __assert_align_res_node[(_Alignof(res_node_h)==_Alignof(struct res_node_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_res_node(void* arg)
{
  #ifdef DV
    #define OPEN_SDC()\
    gpio_set_high(p_self->m_gpio_sdc)
    #define CLOSE_SDC()\
    gpio_set_low(p_self->m_gpio_sdc)
  #else
    #define OPEN_SDC()
    #define CLOSE_SDC()
  #endif // ifdef DV 

  struct res_node_t* const p_self = arg;
  OPEN_SDC();
  p_self->res_state = RES_OFF;
  p_self->remote_running = 1;
  p_self->radio_strenght = 100;
  p_self->lever = 0;
  p_self->button = 0;
  p_self->estop = 1;

  time_var_microseconds t_var = 0;
  can_obj_can3_h_t nmt = {0};
  nmt.can_0x700_DV_RES_On.On = 0;
  uint64_t payload_nmt;
  pack_message_can3(&nmt, CAN_ID_DV_RES_ON, &payload_nmt);
  can_obj_can3_h_t pdo = {0};
  can_obj_can3_h_t operational = {0};

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, get_tick_from_millis(30))
    {
      if(gpio_read_state(&p_self->m_gpio_pcu_dv))
      {
        if(p_self->res_state == RES_OFF)
        {
          OPEN_SDC();
          hardware_mailbox_send(p_self->p_mailbox_send_NMT, payload_nmt);
          p_self->res_state = RES_NMT;
        }
        else if(p_self->res_state == RES_NMT)
        {
          OPEN_SDC();
          CanMessage mex = {0};
          if(hardware_mailbox_read(p_self->p_mailbox_operational, &mex) &&
             unpack_message_can3(&operational, CAN_ID_DV_RES_OPERATIONALMODE, mex.full_word, mex.message_size, 0)>=0)
          {
            if(operational.can_0x000_DV_RES_OperationalMode.RequestedState == 1 &&
              (operational.can_0x000_DV_RES_OperationalMode.Address == 0 || operational.can_0x000_DV_RES_OperationalMode.Address == 0x11))
            {
              p_self->res_state = RES_OPERATING;
            }
          }
        }
        else if(p_self->res_state == RES_OPERATING)
        {
          pdo.can_0x191_DV_RES_Message.SwitchStatus = p_self->lever;
          pdo.can_0x191_DV_RES_Message.ButtonStatus = p_self->button;
          pdo.can_0x191_DV_RES_Message.RadioQuality = p_self->radio_strenght;
          pdo.can_0x191_DV_RES_Message.EStop1 = p_self->estop;
          pdo.can_0x191_DV_RES_Message.EStop2 = p_self->estop;
          uint64_t payload = 0;
          pack_message_can3(&pdo, CAN_ID_DV_RES_MESSAGE, &payload);
          hardware_mailbox_send(p_self->p_mailbox_send_PDO, payload);
          if (p_self->remote_running && p_self->radio_strenght > 50 && !p_self->estop)
          {
            CLOSE_SDC();
          }
          else
          {
            OPEN_SDC();
          }
        }
      }
      else
      {
        p_self->res_state = RES_OFF;
        OPEN_SDC();
      }
    }
  }

  return 0;
}


//public

int8_t res_node_start(res_node_h* const restrict self)
{
  union res_node_h_t_conv conv = {self};
  struct res_node_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_gpio(&p_self->m_gpio_sdc, (enum GPIO_PIN) GPIO_RES_SDC)<0)
  {
    return -1;
  }

  if (hardware_init_read_permission_gpio(&p_self->m_gpio_pcu_dv, GPIO_PCU_DV)<0)
  {
    return -2;
  }

  p_self->p_can_node_dv = hardware_init_new_external_node(CAN_DV);
  if (!p_self->p_can_node_dv)
  {
    return -3;
  }

  p_self->p_mailbox_send_NMT =
    hardware_get_mailbox_single_mex(
      p_self->p_can_node_dv,
      SEND_MAILBOX,
      CAN_ID_DV_RES_ON,
      message_dlc_can3(CAN_ID_DV_RES_ON));

  if (!p_self->p_mailbox_send_NMT)
  {
    hardware_init_new_external_node_destroy(p_self->p_can_node_dv);
    return -4;
  }

  p_self->p_mailbox_send_PDO =
    hardware_get_mailbox_single_mex(
      p_self->p_can_node_dv,
      SEND_MAILBOX,
      CAN_ID_DV_RES_MESSAGE,
      message_dlc_can3(CAN_ID_DV_RES_MESSAGE));

  if (!p_self->p_mailbox_send_PDO)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_NMT);
    hardware_init_new_external_node_destroy(p_self->p_can_node_dv);
    return -5;
  }

  p_self->p_mailbox_operational =
    hardware_get_mailbox_single_mex(
      p_self->p_can_node_dv,
      RECV_MAILBOX,
      CAN_ID_DV_RES_OPERATIONALMODE,
      message_dlc_can3(CAN_ID_DV_RES_OPERATIONALMODE));

  if(!p_self->p_mailbox_operational)
  {
    hardware_free_mailbox_can(&p_self->p_mailbox_send_PDO);
    hardware_free_mailbox_can(&p_self->p_mailbox_send_NMT);
    hardware_init_new_external_node_destroy(p_self->p_can_node_dv);
    return -6;
  }

  p_self->running=1;
  thrd_create(&p_self->thread, _start_res_node, p_self);
  return 0;
}

int8_t res_node_stop(res_node_h* const restrict self)
{
  union res_node_h_t_conv conv = {self};
  struct res_node_t* const p_self = conv.clear;

  printf("stopping res_node\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);
  hardware_free_mailbox_can(&p_self->p_mailbox_send_PDO);
  hardware_free_mailbox_can(&p_self->p_mailbox_send_NMT);
  hardware_free_mailbox_can(&p_self->p_mailbox_operational);
  hardware_init_new_external_node_destroy(p_self->p_can_node_dv);
 
  return 0;
}

