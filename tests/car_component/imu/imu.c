#include "imu.h"
#include "../../linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop

#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>


struct imu_t{
  uint8_t running;
  thrd_t thread;
  int32_t imu_params[(uint8_t) __NUM_OF_IMU_PARAMETERS *(uint8_t) __NUM_OF_AXYS__];
  struct CanNode* p_node_general_can;
};

union imu_h_t_conv{
  imu_h* const hidden;
  struct imu_t* const clear;
};

union imu_h_t_conv_const{
  const imu_h* const hidden;
  const struct imu_t* const clear;
};

#ifdef DEBUG
char __assert_size_imu[(sizeof(imu_h)==sizeof(struct imu_t))?+1:-1];
char __assert_align_imu[(_Alignof(imu_h)==_Alignof(struct imu_t))?+1:-1];
#endif /* ifdef DEBUG */

int _start_imu(void* arg)
{
  struct imu_t* const p_self = arg;
  time_var_microseconds t_var =0;
  can_obj_can2_h_t o2 ={0};
  CanMessage mex ={0};

  const uint8_t pos_acc = IMU_ACCELERATION * __NUM_OF_IMU_PARAMETERS;
  const uint8_t pos_ome = IMU_OMEGA * __NUM_OF_IMU_PARAMETERS;

  while (p_self->running)
  {
    ACTION_ON_FREQUENCY(t_var, 50 MILLIS)
    {
      o2.can_0x060_Imu1.acc_x = p_self->imu_params[pos_acc + IMU_AXES_X];
      o2.can_0x060_Imu1.acc_y = p_self->imu_params[pos_acc + IMU_AXES_Y];
      o2.can_0x061_Imu2.acc_z = p_self->imu_params[pos_acc + IMU_AXES_Z];

      o2.can_0x061_Imu2.omega_x = p_self->imu_params[pos_ome + IMU_AXES_X];
      o2.can_0x062_Imu3.omega_y = p_self->imu_params[pos_ome + IMU_AXES_Y];
      o2.can_0x062_Imu3.omega_z = p_self->imu_params[pos_ome + IMU_AXES_Z];


      mex.id = CAN_ID_IMU1;
      mex.message_size = (uint8_t) pack_message_can2(&o2, mex.id, &mex.full_word);
      hardware_write_can(p_self->p_node_general_can, &mex);

      mex.id = CAN_ID_IMU2;
      mex.message_size = (uint8_t) pack_message_can2(&o2, mex.id, &mex.full_word);
      hardware_write_can(p_self->p_node_general_can, &mex);

      mex.id = CAN_ID_IMU3;
      mex.message_size = (uint8_t) pack_message_can2(&o2, mex.id, &mex.full_word);
      hardware_write_can(p_self->p_node_general_can, &mex);
    }
  }

  return 0;
}


//public

int8_t imu_start(imu_h* const restrict self)
{
  union imu_h_t_conv conv = {self};
  struct imu_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->p_node_general_can = hardware_init_new_external_node(CAN_GENERAL);
  if (!p_self->p_node_general_can)
  {
    return -1;
  }

  p_self->running=1;
  thrd_create(&p_self->thread, _start_imu, p_self);
  return 0;
}

int8_t imu_set_parameter(imu_h* const restrict self, const ImuPharameter param,
    const ImuAxys axes, const int32_t value)
{
  union imu_h_t_conv conv = {self};
  struct imu_t* const p_self = conv.clear;

  if (axes >= __NUM_OF_AXYS__ || param >= __NUM_OF_IMU_PARAMETERS)
  {
    return -1;
  }

  p_self->imu_params[param * __NUM_OF_IMU_PARAMETERS + axes] = value;
  return 0;
}

int8_t imu_stop(imu_h* const restrict self)
{
  union imu_h_t_conv conv = {self};
  struct imu_t* const p_self = conv.clear;

  hardware_init_new_external_node_destroy(p_self->p_node_general_can);

  printf("stopping imu\n");
  p_self->running=0;
  thrd_join(p_self->thread, NULL);

  return 0;
}
