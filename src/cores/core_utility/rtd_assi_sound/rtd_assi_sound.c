#include "rtd_assi_sound.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

struct{
  Gpio_h o_rtd_assi_sound;
  atomic_bool init_done;
}GPIO_RTD_ASSI;

struct RtdAssiSound_t{
  struct Gpio_h* p_gpio_rtd_assi_sound;
};

union RtdAssiSound_h_t_conv {
  RtdAssiSound_h* const hidden;
  struct RtdAssiSound_t* const clear;
};

#ifdef DEBUG
char __assert_size_rtd_assi_sound[(sizeof(RtdAssiSound_h)==sizeof(struct RtdAssiSound_t))?+1:-1];
char __assert_align_rtd_assi_sound[(_Alignof(RtdAssiSound_h)==_Alignof(struct RtdAssiSound_t))?+1:-1];

#endif /* ifdef DEBUG */

//public

int8_t
rtd_assi_sound_init(RtdAssiSound_h* const restrict self)
{
  union RtdAssiSound_h_t_conv conv = {self};
  struct RtdAssiSound_t* const p_self = conv.clear;

  if (!atomic_load(&GPIO_RTD_ASSI.init_done))
  {
    hardware_init_gpio(&GPIO_RTD_ASSI.o_rtd_assi_sound, GPIO_RTD_ASSI_SOUND);
  }

  memset(p_self, 0, sizeof(*p_self));

  p_self->p_gpio_rtd_assi_sound = &GPIO_RTD_ASSI.o_rtd_assi_sound;

  return 0;
}

int8_t
rtd_assi_sound_start(RtdAssiSound_h* const restrict self)
{
  union RtdAssiSound_h_t_conv conv = {self};
  struct RtdAssiSound_t* const p_self = conv.clear;

  gpio_set_low(p_self->p_gpio_rtd_assi_sound);

  return 0;
}

int8_t
rtd_assi_sound_stop(RtdAssiSound_h* const restrict self)
{
  union RtdAssiSound_h_t_conv conv = {self};
  struct RtdAssiSound_t* const p_self = conv.clear;

  if (!GPIO_RTD_ASSI.init_done) {
  
  }

  gpio_set_high(p_self->p_gpio_rtd_assi_sound);

  return 0;
}
