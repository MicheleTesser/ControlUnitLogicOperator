#include "shared_message.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#pragma GCC diagnostic pop

#include <stdatomic.h>
#include <stdint.h>

static struct{
  atomic_ullong data_buffer[__NUM_OF_SHARED_MESSAGE__];
  atomic_ullong timestamp[__NUM_OF_SHARED_MESSAGE__];
  atomic_bool taken;
}SHARED_DATA;

static uint16_t can_id[]={
  CAN_ID_DV_MISSION,
  CAN_ID_LEM,
  CAN_ID_PCU,
  CAN_ID_EMBEDDEDALIVECHECK,
  CAN_ID_DV_RES_ON,
  CAN_ID_EBSSTATUS,
  CAN_ID_DRIVER,
  CAN_ID_DV_DRIVER,
  CAN_ID_IMU1,
  CAN_ID_IMU2,
  CAN_ID_IMU3,
  CAN_ID_CARMISSION
};

struct shared_message_owner_t{
  struct CanMailbox *mailbox[__NUM_OF_SHARED_MESSAGE__];
};

struct shared_message_reader_t{
  enum SHARED_MESSAGE id;
};

union SharedMessageOwner_h_t_conv{
  SharedMessageOwner_h* const restrict hidden;
  struct shared_message_owner_t* const restrict clear;
};

union SharedMessageReader_h_t_conv{
  SharedMessageReader_h* const restrict hidden;
  struct shared_message_reader_t* const restrict clear;
};

union SharedMessageReader_h_t_conv_const{
  const SharedMessageReader_h* const restrict hidden;
  const struct shared_message_reader_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_shared_reader[sizeof(SharedMessageReader_h) == sizeof(struct shared_message_reader_t)? 1:-1];
char __assert_size_owner[sizeof(SharedMessageOwner_h) == sizeof(struct shared_message_owner_t)? 1:-1];
char __assert_align_shared_reader[_Alignof(SharedMessageReader_h) == _Alignof(struct shared_message_reader_t)? 1:-1];
char __assert_align_shared_owner[_Alignof(SharedMessageOwner_h) == _Alignof(struct shared_message_owner_t)? 1:-1];
#endif //! DEBUG

// public

int8_t shared_message_owner_init(SharedMessageOwner_h* const restrict self)
{
  if (atomic_load(&SHARED_DATA.taken))
  {
    return -1;
  }
  atomic_store(&SHARED_DATA.taken, 1);

  union SharedMessageOwner_h_t_conv conv = {self};
  struct shared_message_owner_t* p_self = conv.clear;

  struct CanNode *can_node = NULL;

  ACTION_ON_CAN_NODE (CAN_GENERAL, can_node)
  {
    p_self->mailbox[SHARED_MEX_DV_MISSION] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_DV_MISSION],
        message_dlc_can2(can_id[SHARED_MEX_DV_MISSION]));
    p_self->mailbox[SHARED_MEX_LEM] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_LEM],
        message_dlc_can2(can_id[SHARED_MEX_LEM]));
    p_self->mailbox[SHARED_MEX_PCU] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_PCU],
        message_dlc_can2(can_id[SHARED_MEX_PCU]));
    p_self->mailbox[SHARED_MEX_EMBEDDEDALIVECHECK] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_EMBEDDEDALIVECHECK],
        message_dlc_can2(can_id[SHARED_MEX_EMBEDDEDALIVECHECK]));
    p_self->mailbox[SHARED_MEX_DV_RES_ON] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_DV_RES_ON],
        message_dlc_can2(can_id[SHARED_MEX_DV_RES_ON]));
    p_self->mailbox[SHARED_MEX_EBSSTATUS] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_EBSSTATUS],
        message_dlc_can2(can_id[SHARED_MEX_EBSSTATUS]));
    p_self->mailbox[SHARED_MEX_DRIVER] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_DRIVER],
        message_dlc_can2(can_id[SHARED_MEX_DRIVER]));
    p_self->mailbox[SHARED_MEX_DV_DRIVER] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_DV_DRIVER],
        message_dlc_can2(can_id[SHARED_MEX_DV_DRIVER]));
    p_self->mailbox[SHARED_MEX_IMU1] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_IMU1],
        message_dlc_can2(can_id[SHARED_MEX_IMU1]));
    p_self->mailbox[SHARED_MEX_IMU2] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_IMU2],
        message_dlc_can2(can_id[SHARED_MEX_IMU2]));
    p_self->mailbox[SHARED_MEX_IMU3] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_IMU3],
        message_dlc_can2(can_id[SHARED_MEX_IMU3]));
    p_self->mailbox[SHARED_MEX_CARMISSION] =
      hardware_get_mailbox_single_mex (
        can_node,
        RECV_MAILBOX,
        can_id[SHARED_MEX_CARMISSION],
        message_dlc_can2(can_id[SHARED_MEX_CARMISSION]));
  }
  for(uint8_t i = 0; i < __NUM_OF_SHARED_MESSAGE__; ++i)
  {
    if(!p_self->mailbox[i])
    {
      return -2;
    }
  }

  return 0;
}

int8_t shared_message_reader_init(SharedMessageReader_h* const restrict self,
                            const enum SHARED_MESSAGE id)
{
  union SharedMessageReader_h_t_conv conv = {self};
  struct shared_message_reader_t* p_self = conv.clear;
  p_self->id = id;
  return 0;
}

uint64_t shared_message_read(const SharedMessageReader_h* const restrict self, uint64_t* const message)
{
  const union SharedMessageReader_h_t_conv_const conv = {self};
  const struct shared_message_reader_t* p_self = conv.clear;

  *message = atomic_load(&SHARED_DATA.data_buffer[p_self->id]);
  return atomic_load(&SHARED_DATA.timestamp[p_self->id]);
}

int8_t shared_message_owner_update(SharedMessageOwner_h* const restrict self)
{
  const union SharedMessageOwner_h_t_conv conv = {self};
  const struct shared_message_owner_t* p_self = conv.clear;

  CanMessage mex = {0};
  can_obj_can2_h_t o2 = {0};
  
  for(uint8_t i = 0; i < __NUM_OF_SHARED_MESSAGE__; ++i)
  {
    if(hardware_mailbox_read(p_self->mailbox[i], &mex))
    {
        unpack_message_can2(&o2, mex.id, mex.full_word, mex.message_size, 0);
        atomic_store(&SHARED_DATA.data_buffer[i], mex.full_word);
        atomic_store(&SHARED_DATA.timestamp[i], atomic_load(&SHARED_DATA.timestamp[i]) + 1);
    }
  }
  return 0;
}



