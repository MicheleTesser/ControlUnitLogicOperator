#include "shared_message.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdatomic.h>
#include <stdint.h>

static struct{
  atomic_ullong data_buffer[__NUM_OF_SHARED_MESSAGE__];
  atomic_ullong timestamp[__NUM_OF_SHARED_MESSAGE__];
  atomic_bool taken;
}SHARED_DATA;


#define SPEC_MEX_CAN_2(mex_id)\
  {mex_id ,CAN_GENERAL}
#define SPEC_MEX_CAN_3(mex_id)\
  {mex_id ,CAN_DV}

static const struct{
  uint16_t can_id;
  enum CAN_MODULES can_mod;
}can_id[__NUM_OF_SHARED_MESSAGE__]={
  SPEC_MEX_CAN_3(CAN_ID_DV_MISSION),
  SPEC_MEX_CAN_2(CAN_ID_LEM),
  SPEC_MEX_CAN_2(CAN_ID_PCU),
  SPEC_MEX_CAN_2(CAN_ID_EMBEDDEDALIVECHECK),
  SPEC_MEX_CAN_3(CAN_ID_DV_RES_ON),
  SPEC_MEX_CAN_2(CAN_ID_EBSSTATUS),
  SPEC_MEX_CAN_2(CAN_ID_DRIVER),
  SPEC_MEX_CAN_3(CAN_ID_DV_DRIVER),
  SPEC_MEX_CAN_2(CAN_ID_IMU1),
  SPEC_MEX_CAN_2(CAN_ID_IMU2),
  SPEC_MEX_CAN_2(CAN_ID_IMU3),
  SPEC_MEX_CAN_2(CAN_ID_CARMISSION),
};

struct shared_message_owner_t{
  struct CanMailbox *p_mailbox[__NUM_OF_SHARED_MESSAGE__];
};

struct shared_message_reader_t{
  enum SHARED_MESSAGE m_id;
  time_var_microseconds m_last_read;
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

static inline int8_t _init_mailbox_ext_mex(struct shared_message_owner_t* const restrict self,
    struct CanNode* can_node,
    const enum SHARED_MESSAGE mex)
{
  uint8_t size=0;
  const uint16_t m_id= can_id[mex].can_id;

  switch (can_id[mex].can_mod)
  {
    case CAN_INVERTER:
      size = (uint8_t) message_dlc_can1(m_id);
      break;
    case CAN_GENERAL:
      size = (uint8_t) message_dlc_can2(m_id);
      break;
    case CAN_DV:
      size = (uint8_t) message_dlc_can3(m_id);
      break;
    case CAN_DEBUG:
    case __NUM_OF_CAN_MODULES__:
      return -1;
      break;
  }

  self->p_mailbox[mex] =
    hardware_get_mailbox_single_mex(
        can_node,
        RECV_MAILBOX,
        m_id,
        size);

  return !self->p_mailbox[mex]?-1:0;
}

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
  int8_t err=0;


  ACTION_ON_CAN_NODE (CAN_GENERAL, can_node)
  {
    for (enum SHARED_MESSAGE mex=0; mex<__NUM_OF_SHARED_MESSAGE__; mex++)
    {
      if (_init_mailbox_ext_mex(p_self, can_node, mex)<0)
      {
        err =-1;
        break;
      }
    }
  }

  if (err<0)
  {
    for(enum SHARED_MESSAGE mex = 0; mex < __NUM_OF_SHARED_MESSAGE__; ++mex)
    {
      if(p_self->p_mailbox[mex])
      {
        hardware_free_mailbox_can(&p_self->p_mailbox[mex]);
      }
    }
    return -1;
  }

  return 0;
}

int8_t shared_message_owner_status(void)
{
  return atomic_load(&SHARED_DATA.taken);
}


int8_t shared_message_reader_init(SharedMessageReader_h* const restrict self,
                            const enum SHARED_MESSAGE id)
{
  if(id >= __NUM_OF_SHARED_MESSAGE__ || id < 0)
  {
    return -1;
  }
  union SharedMessageReader_h_t_conv conv = {self};
  struct shared_message_reader_t* p_self = conv.clear;
  p_self->m_id = id;
  return 0;
}

int8_t shared_message_read(SharedMessageReader_h* const restrict self, uint64_t* const message)
{
  union SharedMessageReader_h_t_conv conv = {self};
  struct shared_message_reader_t* p_self = conv.clear;

  const time_var_microseconds current_timestamp = atomic_load(&SHARED_DATA.timestamp[p_self->m_id]);

  if (p_self->m_last_read < current_timestamp)
  {
    p_self->m_last_read = current_timestamp;
    *message = atomic_load(&SHARED_DATA.data_buffer[p_self->m_id]);
    return 1;
  }

  return 0;
}

int8_t shared_message_owner_update(SharedMessageOwner_h* const restrict self)
{
  const union SharedMessageOwner_h_t_conv conv = {self};
  const struct shared_message_owner_t* p_self = conv.clear;

  CanMessage mex = {0};

  for(uint8_t i = 0; i < __NUM_OF_SHARED_MESSAGE__; ++i)
  {
    if(hardware_mailbox_read(p_self->p_mailbox[i], &mex))
    {
        atomic_store(&SHARED_DATA.data_buffer[i], mex.full_word);
        atomic_store(&SHARED_DATA.timestamp[i], timer_time_now());
    }
  }
  return 0;
}

int8_t shared_message_read_unpack_can1(SharedMessageReader_h* const restrict self, can_obj_can1_h_t* o1)
{
  const union SharedMessageReader_h_t_conv conv = {self};
  const struct shared_message_reader_t* p_self = conv.clear;

  uint64_t mex;
  if(shared_message_read(self, &mex))
  {
    unpack_message_can1(o1, can_id[p_self->m_id].can_id, mex, message_dlc_can1(can_id[p_self->m_id].can_id), p_self->m_last_read);
    return 1;
  }
  return 0;
}

int8_t shared_message_read_unpack_can2(SharedMessageReader_h* const restrict self, can_obj_can2_h_t* o2)
{
  const union SharedMessageReader_h_t_conv conv = {self};
  const struct shared_message_reader_t* p_self = conv.clear;

  uint64_t mex;
  if(shared_message_read(self, &mex))
  {
    unpack_message_can2(o2, can_id[p_self->m_id].can_id, mex, message_dlc_can2(can_id[p_self->m_id].can_id), p_self->m_last_read);
    return 1;
  }
  return 0;
}

int8_t shared_message_read_unpack_can3(SharedMessageReader_h* const restrict self, can_obj_can3_h_t* o3)
{
  const union SharedMessageReader_h_t_conv conv = {self};
  const struct shared_message_reader_t* p_self = conv.clear;

  uint64_t mex;
  if(shared_message_read(self, &mex))
  {
    unpack_message_can3(o3, can_id[p_self->m_id].can_id, mex, message_dlc_can3(can_id[p_self->m_id].can_id), p_self->m_last_read);
    return 1;
  }
  return 0;
}
