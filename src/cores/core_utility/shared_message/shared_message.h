#ifndef __CORE_SHARED_MESSAGE__
#define __CORE_SHARED_MESSAGE__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can1/can1.h"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#pragma GCC diagnostic pop

#include <stdint.h>

#include "../../../../lib/raceup_board/raceup_board.h"

enum SHARED_MESSAGE{
  SHARED_MEX_DV_MISSION = 0,
  SHARED_MEX_LEM,
  SHARED_MEX_PCU,
  SHARED_MEX_EMBEDDEDALIVECHECK,
  SHARED_MEX_DV_RES_ON,
  SHARED_MEX_EBSSTATUS,
  SHARED_MEX_DRIVER,
  SHARED_MEX_DV_DRIVER,
  SHARED_MEX_IMU1,
  SHARED_MEX_IMU2,
  SHARED_MEX_IMU3,
  SHARED_MEX_CARMISSION,

  __NUM_OF_SHARED_MESSAGE__
};

typedef struct __attribute__((__aligned__ (4))){
  const uint8_t private_data[48];
}SharedMessageOwner_h;

typedef struct __attribute__((__aligned__ (4))){
  const uint8_t private_data[12];
}SharedMessageReader_h;

int8_t
shared_message_owner_init(SharedMessageOwner_h* const restrict self)__attribute__ ((__nonnull__ (1)));

int8_t
shared_message_owner_update(SharedMessageOwner_h* const restrict self)__attribute__ ((__nonnull__ ((1))));

int8_t
shared_message_reader_init(SharedMessageReader_h* const restrict self, const enum SHARED_MESSAGE id)__attribute__((__nonnull__((1))));

int8_t
shared_message_owner_status(void);

int8_t
shared_message_read(SharedMessageReader_h* const restrict self, uint64_t* const message)__attribute__((__nonnull__(1, 2)));

int8_t
shared_message_read_unpack_can1(SharedMessageReader_h* const restrict self, can_obj_can1_h_t* o1);

int8_t
shared_message_read_unpack_can2(SharedMessageReader_h* const restrict self, can_obj_can2_h_t* o2);

int8_t
shared_message_read_unpack_can3(SharedMessageReader_h* const restrict self, can_obj_can3_h_t* o3);

#endif // !__CORE_SHARED_MESSAGE__
