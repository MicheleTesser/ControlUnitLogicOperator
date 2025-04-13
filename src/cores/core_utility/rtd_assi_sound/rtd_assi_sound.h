#ifndef __RTD_ASSI_SOUND__
#define __RTD_ASSI_SOUND__

#include <stdint.h>
#include "../../../lib/raceup_board/raceup_board.h"

#if ARCH == 64
typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[16];
}RtdAssiSound_h;
#elif ARCH == 32
typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[5];
}RtdAssiSound_h;
#else
#endif

void
rtd_assi_sound_init(RtdAssiSound_h* const restrict self)__attribute__((__nonnull__(1)));

void
rtd_assi_sound_start(RtdAssiSound_h* const restrict self)__attribute__((__nonnull__(1)));

void
rtd_assi_sound_stop(RtdAssiSound_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__RTD_ASSI_SOUND__
