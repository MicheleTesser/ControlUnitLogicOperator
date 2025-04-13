#ifndef __DRIVER_INPUT_READER__
#define __DRIVER_INPUT_READER__

#include <stdint.h>
#include "../../../lib/raceup_board/raceup_board.h"

#if ARCH == 64
typedef struct __attribute__((aligned(8))){
  uint8_t private_data[40];
}DriverInputReader_h;
#elif ARCH == 32
typedef struct __attribute__((aligned(4))){
  uint8_t private_data[32];
}DriverInputReader_h;
#else
#endif

enum INPUT_TYPES{
    THROTTLE =0,
    BRAKE,
    STEERING_ANGLE,

    __NUM_OF_INPUT_TYPES__
};

enum DRIVER{
    DRIVER_HUMAN=0,
    DRIVER_EMBEDDED,

    DRIVER_NONE,
    __NUM_OF_DRIVERS__
};

int8_t
driver_input_reader_init(DriverInputReader_h* const restrict self)__attribute__((__nonnull__(1)));

float
driver_input_reader_get(const DriverInputReader_h* const restrict self,
    const enum DRIVER driver_type,
    const enum INPUT_TYPES driver_input)__attribute__((__nonnull__(1)));

int8_t
driver_input_reader_update(DriverInputReader_h* const restrict self)__attribute__((__nonnull__(1)));

void
driver_input_reader_destroy(DriverInputReader_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DRIVER_INPUT_READER__
