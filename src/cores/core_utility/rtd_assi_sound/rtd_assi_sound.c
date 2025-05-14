#include "rtd_assi_sound.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>


struct RtdAssiSound_t{
  uint8_t m_desidered_state:1;
  uint8_t m_req_change:1;
  uint8_t m_user_id:6;
  Gpio_h* p_gpio_rtd_assi_sound;
};

union RtdAssiSound_h_t_conv {
  RtdAssiSound_h* const hidden;
  struct RtdAssiSound_t* const clear;
};

struct{
  Gpio_h m_rtd_assi_sound;
  atomic_char m_on_req;
  atomic_bool m_init_done;
}GPIO_RTD_ASSI;

#ifdef DEBUG
char __assert_size_rtd_assi_sound[(sizeof(RtdAssiSound_h)==sizeof(struct RtdAssiSound_t))?+1:-1];
char __assert_align_rtd_assi_sound[(_Alignof(RtdAssiSound_h)==_Alignof(struct RtdAssiSound_t))?+1:-1];

#endif /* ifdef DEBUG */

//public

int8_t rtd_assi_sound_init(RtdAssiSound_h* const restrict self)
{
  union RtdAssiSound_h_t_conv conv = {self};
  struct RtdAssiSound_t* const p_self = conv.clear;

  if (!atomic_load(&GPIO_RTD_ASSI.m_init_done))
  {
    atomic_store(&GPIO_RTD_ASSI.m_init_done, 1);
    hardware_init_gpio(&GPIO_RTD_ASSI.m_rtd_assi_sound, GPIO_RTD_ASSI_SOUND);
  }

  memset(p_self, 0, sizeof(*p_self));

  p_self->p_gpio_rtd_assi_sound = &GPIO_RTD_ASSI.m_rtd_assi_sound;
  return 0;
}

int8_t rtd_assi_sound_start(RtdAssiSound_h* const restrict self)
{
  union RtdAssiSound_h_t_conv conv = {self};
  struct RtdAssiSound_t* const p_self = conv.clear;

  p_self->m_desidered_state = 1;
  if (!p_self->m_req_change)
  {
    p_self->m_req_change=1;
    atomic_store(&GPIO_RTD_ASSI.m_on_req, atomic_load(&GPIO_RTD_ASSI.m_on_req) + 1);
    gpio_set_low(p_self->p_gpio_rtd_assi_sound);
  }
  return 0;
}

int8_t rtd_assi_sound_stop(RtdAssiSound_h* const restrict self)
{
  union RtdAssiSound_h_t_conv conv = {self};
  struct RtdAssiSound_t* const p_self = conv.clear;
  uint8_t on_req =atomic_load(&GPIO_RTD_ASSI.m_on_req);

  p_self->m_desidered_state = 0;
  if(p_self->m_req_change &&
      on_req == 1
      &&p_self->m_desidered_state == on_req)
  {
    p_self->m_req_change=0;
    atomic_store(&GPIO_RTD_ASSI.m_on_req,0);
    gpio_set_high(p_self->p_gpio_rtd_assi_sound);
  }
  return 0;
}
