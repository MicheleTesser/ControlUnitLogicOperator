#include "core_1_imu.h"
#include "../log/log.h"
#include "../../../../lib/raceup_board/raceup_board.h"
#include "../../../core_utility/imu/imu.h"
#include "../../../core_utility/car_speed/car_speed.h"

#include <stdint.h>
#include <string.h>

enum IMU_DATA{
  ACC_X=0,
  ACC_Y,
  ACC_Z,
  OMEGA_X,
  OMEGA_Y,
  OMEGA_Z,

  __NUM_OF_IMU_DATA__
};

struct Core1Imu_t{
  float imu_data[__NUM_OF_IMU_DATA__];
  uint32_t speed;
  Imu_h m_imu;
};

union Core1Imu_h_t_conv {
  Core1Imu_h* const restrict hidden;
  struct Core1Imu_t* const restrict clear;
};

//private

#ifdef DEBUG
uint8_t __assert_size_core_1_imu[(sizeof(Core1Imu_h) == sizeof(struct Core1Imu_t))?1:-1];
uint8_t __assert_align_core_1_imu[(_Alignof(Core1Imu_h) == _Alignof(struct Core1Imu_t))?1:-1];
#endif /* ifdef DEBUG */

#define IMU_LOG_VAR(var_ptr, var_name, err_code)\
{\
  LogEntry_h log_entry={0};\
  log_entry.data_mode = (LOG_TELEMETRY | LOG_SD);\
  log_entry.data_ptr = var_ptr;\
  log_entry.data_format= "%.02f";\
  memcpy(log_entry.name, var_name, strlen(var_name));\
  if(log_add_entry(log, &log_entry)<0)return err_code;\
}

int8_t core_1_imu_init(Core1Imu_h* const restrict self,
    Log_h* const restrict log)
{
  union Core1Imu_h_t_conv conv = {self};
  struct Core1Imu_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (imu_init(&p_self->m_imu)<0)
  {
    return -1;
  }

  IMU_LOG_VAR(&p_self->imu_data[ACC_X], "imu acc x",-1);
  IMU_LOG_VAR(&p_self->imu_data[ACC_Y], "imu acc y",-2);
  IMU_LOG_VAR(&p_self->imu_data[ACC_Z], "imu acc z",-3);
  IMU_LOG_VAR(&p_self->speed, "car speed",-4);

  return 0;
}

int8_t core_1_imu_update(Core1Imu_h* const restrict self)
{
  union Core1Imu_h_t_conv conv = {self};
  struct Core1Imu_t* const restrict p_self = conv.clear;

  p_self->imu_data[ACC_X] = imu_get_acc(&p_self->m_imu, AXES_X);
  p_self->imu_data[ACC_Y] = imu_get_acc(&p_self->m_imu, AXES_Y);
  p_self->imu_data[ACC_Z] = imu_get_acc(&p_self->m_imu, AXES_Z);
  p_self->speed = car_speed_get();

  //TODO: add omegas

  return 0;
}
