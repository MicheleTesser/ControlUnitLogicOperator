#include "imu.h"
#include <stdint.h>
#include <string.h>
#include "../../../../../../lib/raceup_board/components/can.h"

typedef struct DvImu_t{
  struct CanMailbox* imu_mailbox;
}DvImu_t;

union DvImu_h_t_conv {
  DvImu_h* const restrict hidden;
  DvImu_t* const restrict clear;
};

#ifdef DEBUG
uint8_t __assert_size_dv_imu[(sizeof(DvImu_h) == sizeof(struct DvImu_t))? 1 : -1];
uint8_t __assert_align_dv_imu[(_Alignof(DvImu_h) == _Alignof(struct DvImu_t))? 1 : -1];
#endif // DEBUG

int8_t
dv_imu_init(DvImu_h* const restrict self)
{
  union DvImu_h_t_conv conv = {self};
  struct DvImu_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  //TODO: init the mailbox

  return 0;
}

int8_t
dv_imu_update(DvImu_h* const restrict self __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return -1;
}

int8_t
dv_imu_get(DvImu_h* const restrict self __attribute__((__unused__)),
    const enum DV_IMU_DATA data __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return -1;
}
