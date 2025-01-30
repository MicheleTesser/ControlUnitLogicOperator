#include "core_1_imu.h"
#include "../log/log.h"
#include "../../../../lib/raceup_board/raceup_board.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

enum IMU_DATA{
  ACC_X=0,
  ACC_Y,
  ACC_Z,
  OMEGA_X,
  OMEGA_Y,
  OMEGA_Z,
  SPEED,

  __NUM_OF_IMU_DATA__
};

struct Core1Imu_t{
  struct CanMailbox* imu_mailbox;
  float imu_data[__NUM_OF_IMU_DATA__];
};

union Core1Imu_h_t_conv {
  Core1Imu_h* const restrict hidden;
  struct Core1Imu_t* const restrict clear;
};

//private

#ifdef DEBUG
uint8_t __assert_size_core_1_imu[(sizeof(Core1Imu_h) == sizeof(struct Core1Imu_t))?1:-1];
#endif /* ifdef DEBUG */

int8_t
core_1_imu_init(
        Core1Imu_h* const restrict self ,
        Log_h* const restrict log )
{
  union Core1Imu_h_t_conv conv = {self};
  struct Core1Imu_t* const restrict p_self = conv.clear;
  LogEntry_h log_entry={0};

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE(CAN_GENERAL,{
    // p_self->imu_mailbox=hardware_get_mailbox(can_node, -1, -1);//TODO: not yet defined
  })

  log_entry.data_max = 100; //TODO: define max 
  log_entry.data_min = 100; //TODO: define min 
  log_entry.data_mode = (LOG_TELEMETRY | LOG_SD);
  log_entry.data_ptr = &p_self->imu_data[ACC_X];

  const char* const imu_acc_x_name = "imu acc x";
  memcpy(log_entry.name, imu_acc_x_name , strlen(imu_acc_x_name));
  log_add_entry(log, &log_entry);

  return 0;
}

int8_t
core_1_imu_update(Core1Imu_h* const restrict self)
{
  union Core1Imu_h_t_conv conv = {self};
  struct Core1Imu_t* const restrict p_self __attribute__((__unused__))= conv.clear;

  return 0;
}
