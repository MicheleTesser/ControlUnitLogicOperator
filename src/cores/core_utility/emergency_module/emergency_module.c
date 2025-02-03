#include "./emergency_module.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

//private

#define NUM_EMERGENCY_BUFFER 8

struct EmergencyNode_t
{
  uint8_t emergency_buffer[NUM_EMERGENCY_BUFFER];
  uint8_t emergency_counter;
  uint8_t *emergency_state;
};

union EmergencyNode_h_t_conv
{
  EmergencyNode_h* const restrict hidden;
  struct EmergencyNode_t* const restrict clear;
};

union EmergencyNode_h_t_conv_const
{
  const EmergencyNode_h* const restrict hidden;
  const struct EmergencyNode_t* const restrict clear;
};

static struct{
  atomic_flag lock;
  Gpio_h gpio_scs;
  uint8_t excepion_counter;
  uint8_t init_done:1;
}EXCEPTION_COUNTER;

static void raise_module_exception_state(void) TRAP_ATTRIBUTE
{
  while (atomic_flag_test_and_set(&EXCEPTION_COUNTER.lock));
  EXCEPTION_COUNTER.excepion_counter++;
  gpio_set_low(&EXCEPTION_COUNTER.gpio_scs);
  atomic_flag_clear(&EXCEPTION_COUNTER.lock);
}

static void solved_module_exception_state(void) TRAP_ATTRIBUTE
{
  while (atomic_flag_test_and_set(&EXCEPTION_COUNTER.lock));
  EXCEPTION_COUNTER.excepion_counter--;
  if (!EXCEPTION_COUNTER.excepion_counter)
  {
    gpio_set_high(&EXCEPTION_COUNTER.gpio_scs);
  }
  atomic_flag_clear(&EXCEPTION_COUNTER.lock);
}

#ifdef DEBUG
uint8_t __assert_size_emergency_node[(sizeof(EmergencyNode_h)==sizeof(struct EmergencyNode_t))?1:-1];
#endif /* ifdef DEBUG */

//public

int8_t EmergencyNode_class_init(void)
{
  if (hardware_init_gpio(&EXCEPTION_COUNTER.gpio_scs, GPIO_SCS)<0)
  {
    return -1;   
  }
  EXCEPTION_COUNTER.init_done=1;

  return 0;
}

int8_t
EmergencyNode_init(EmergencyNode_h* const restrict self)
{
  union EmergencyNode_h_t_conv conv = {self};
  struct EmergencyNode_t* const restrict p_self =conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  hardware_trap_attach_fun(EMERGENCY_RAISED_TRAP, raise_module_exception_state);
  hardware_trap_attach_fun(EMERGENCY_SOLVED_TRAP, solved_module_exception_state);
  p_self->emergency_state = &EXCEPTION_COUNTER.excepion_counter;

  return 0;
}

int8_t EmergencyNode_raise(struct EmergencyNode_h* const restrict self, const uint8_t exeception)
{
  const uint8_t exception_byte = exeception/8;
  const uint8_t exception_bit = exeception % 8;
  union EmergencyNode_h_t_conv conv = {self};
  struct EmergencyNode_t* const restrict p_self =conv.clear;

  if (exeception >= NUM_EMERGENCY_BUFFER*8)
  {
    return -1;
  }

  p_self->emergency_buffer[exception_byte] |= 1 << exception_bit;
  p_self->emergency_counter++;

  if (!p_self->emergency_counter) {
    hardware_raise_trap(EMERGENCY_RAISED_TRAP);
  }

  return 0;
}

int8_t EmergencyNode_solve(struct EmergencyNode_h* const restrict self, const uint8_t exeception)
{
  const uint8_t exception_byte = exeception/8;
  const uint8_t exception_bit = 1 << (exeception % 8);
  union EmergencyNode_h_t_conv conv = {self};
  struct EmergencyNode_t* const restrict p_self =conv.clear;

  if (exeception >= NUM_EMERGENCY_BUFFER * 8)
  {
    return -1;
  }

  if (p_self->emergency_buffer[exception_byte] &  exception_bit)
  {
    p_self->emergency_buffer[exception_byte] ^= exception_bit;
    p_self->emergency_counter--;
  }

  if (!p_self->emergency_counter)
  {
    hardware_raise_trap(EMERGENCY_SOLVED_TRAP);
  }
  
  return 0;
}

int8_t EmergencyNode_is_emergency_state(const struct EmergencyNode_h* const restrict self)
{
  const union EmergencyNode_h_t_conv_const conv = {self};
  const struct EmergencyNode_t* const restrict p_self =conv.clear;
  return *p_self->emergency_state;
}
