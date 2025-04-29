#ifndef __DUMMY__
#define __DUMMY__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[60];
}Asb_h;

//INFO: phase details at: https://github.com/raceup-electric/EBS/tree/main
typedef enum{
  EbsPhase_0=0,
  EbsPhase_1,
  EbsPhase_2,
  EbsPhase_3,
  EbsPhase_4,
  EbsPhase_5,
}EbsPhaes_t;


enum ASB_CONFIG{
  TANK_LEFT_PRESSURE=0,
  TANK_LEFT_SANITY,

  TANK_RIGHT_PRESSURE,
  TANK_RIGHT_SANITY,

  SYSTEM_CHECK,
  INTEGRITY_CHECK_STATUS,

  CURR_MISSION,

  __NUM_OF_ASB_CONFI__
};

int8_t
asb_start(Asb_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
asb_set_parameter(Asb_h* const restrict self,
    const enum ASB_CONFIG param_type, const uint8_t value)__attribute__((__nonnull__(1)));

void
asb_reset(Asb_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
asb_stop(Asb_h* const restrict self)__attribute__((__nonnull__(1)));

EbsPhaes_t
asb_current_phase(const Asb_h* const restrict self)__attribute__((__nonnull__(1)));


#endif // !__DUMMY__
