#include "feature.h"
#include "driver_input/driver_input.h"
#include "engines/amk/amk.h"
#include "engines/engines.h"
#include "giei/giei.h"
#include "imu/imu.h"
#include "maps/maps.h"
#include "../../core_utility/running_status/running_status.h"

#include <stdint.h>

struct Core0Feature_t{
  DriverInput_h driver;
  DrivingMaps_h maps;
  Imu_h imu;
  AmkInverter_h amk;
  EngineType inverter;
  Giei_h giei;
  CarMissionReader_h o_car_mission_reader;
  enum RUNNING_STATUS old_running_status;
};

union Core0Feature_h_t_conv{
  Core0Feature_h* const restrict hidden;
  struct Core0Feature_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_0_feature[(sizeof(Core0Feature_h)) == sizeof(struct Core0Feature_t)? 1:-1];
char __assert_align_core_0_feature[(_Alignof(Core0Feature_h)) == _Alignof(struct Core0Feature_t)? 1:-1];
#endif // DEBUG

//public

int8_t core_0_feature_init(Core0Feature_h* const restrict self )
{
  union Core0Feature_h_t_conv conv = {self};
  struct Core0Feature_t* const restrict p_self = conv.clear;

  if(car_mission_reader_init(&p_self->o_car_mission_reader)) return -1;
  if(driver_input_init(&p_self->driver, &p_self->o_car_mission_reader) <0) return -2;
  if(driving_maps_init(&p_self->maps) <0) return -3;
  if(imu_init(&p_self->imu) <0) return -4;
  if(amk_module_init(&p_self->amk, &p_self->driver, &p_self->inverter)) return -5;
  if(giei_init(&p_self->giei, &p_self->inverter, &p_self->driver, &p_self->maps,
        &p_self->imu) <0) return -6;

  return 0;
}

int8_t core_0_feature_update(Core0Feature_h* const restrict self )
{
  union Core0Feature_h_t_conv conv = {self};
  struct Core0Feature_t* const restrict p_self = conv.clear;

  if(giei_driver_input_update(&p_self->driver)<0) return -1;
  if(driving_map_update(&p_self->maps) <0) return -3;
  if(imu_update(&p_self->imu) <0) return -4;
  if(giei_update(&p_self->giei)<0)return -5;

  return 0;
}


int8_t core_0_feature_compute_power(Core0Feature_h* const restrict self )
{
  union Core0Feature_h_t_conv conv = {self};
  struct Core0Feature_t* const restrict p_self = conv.clear;
  enum RUNNING_STATUS status = SYSTEM_OFF;

  status = GIEI_check_running_condition(&p_self->giei);
  if (status)
  {
    if (p_self->old_running_status != status) {
      p_self->old_running_status = status;
      if(global_running_status_set(status)<0)
      {
        return -1;
      }
    }
    if(GIEI_compute_power(&p_self->giei)<0)
    {
      return -2;
    }
  }
  return 0;
}
