#include "shared_message.h"
#include "../../../lib/raceup_board/raceup_board.h"
#include <stdatomic.h>
#include <stdint.h>

typedef struct
{
  atomic_bool lock;
  uint64_t data_buffer;
  time_var_microseconds timestamp;
}SharedMex_t;

static struct{
  SharedMex_t m_data[__NUM_OF_SHARED_MESSAGE__];
  atomic_bool m_taken;
}SHARED_DATA;


#define SPEC_MEX_CAN_2(mex_id)\
  {mex_id ,CAN_GENERAL}
#define SPEC_MEX_CAN_3(mex_id)\
  {mex_id ,CAN_DV}

static const struct{
  uint16_t can_id;
  enum CAN_MODULES can_mod;
}can_id[__NUM_OF_SHARED_MESSAGE__]={
  SPEC_MEX_CAN_3(CAN_ID_DV_EMBEDDED_STATUS), // (0)
  SPEC_MEX_CAN_2(CAN_ID_LEM),// (1)
  SPEC_MEX_CAN_2(CAN_ID_PCU),// (2)
  SPEC_MEX_CAN_2(CAN_ID_EMBEDDEDALIVECHECK),// (3)
  SPEC_MEX_CAN_3(CAN_ID_DV_RES_ON),// (4)
  SPEC_MEX_CAN_2(CAN_ID_EBSSTATUS),// (5)
  SPEC_MEX_CAN_2(CAN_ID_DRIVER),// (6)
  SPEC_MEX_CAN_3(CAN_ID_DV_DRIVER),// (7)
  SPEC_MEX_CAN_2(CAN_ID_IMU1),// (8)
  SPEC_MEX_CAN_2(CAN_ID_IMU2),// (9)
  SPEC_MEX_CAN_2(CAN_ID_IMU3),// (10)
  SPEC_MEX_CAN_2(CAN_ID_CARMISSION),// (11)
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
    const enum SHARED_MESSAGE mex)
{
  uint8_t size=0;
  const uint16_t m_id= can_id[mex].can_id;
  struct CanNode* can_node = NULL;

  ACTION_ON_CAN_NODE (can_id[mex].can_mod, can_node)
  {
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
  }

  return !self->p_mailbox[mex]?-1:0;
}

// public

int8_t shared_message_owner_init(SharedMessageOwner_h* const restrict self)
{
  if (atomic_load(&SHARED_DATA.m_taken))
  {
    return -1;
  }
  atomic_store(&SHARED_DATA.m_taken, 1);

  union SharedMessageOwner_h_t_conv conv = {self};
  struct shared_message_owner_t* p_self = conv.clear;
  int8_t err=0;


  for (enum SHARED_MESSAGE mex=0; mex<__NUM_OF_SHARED_MESSAGE__; mex++)
  {
    if (_init_mailbox_ext_mex(p_self, mex)<0)
    {
      err =-1;
      break;
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
  return atomic_load(&SHARED_DATA.m_taken);
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
  SharedMex_t* shared_mex = &SHARED_DATA.m_data[p_self->m_id];

  if (atomic_load(&shared_mex->lock))
  {
    return -1;
  }

  const time_var_microseconds current_timestamp = shared_mex->timestamp;

  if (p_self->m_last_read < current_timestamp)
  {
    p_self->m_last_read = current_timestamp;
    *message =shared_mex->data_buffer;
    return 1;
  }

  return 0;
}

int8_t shared_message_owner_update(SharedMessageOwner_h* const restrict self)
{
  const union SharedMessageOwner_h_t_conv conv = {self};
  const struct shared_message_owner_t* p_self = conv.clear;
  SharedMex_t* shared_mex = NULL;
  CanMessage mex = {0};


  for(uint8_t i = 0; i < __NUM_OF_SHARED_MESSAGE__; ++i)
  {
    shared_mex = &SHARED_DATA.m_data[i];
    atomic_store(&shared_mex->lock, 1);
    if(hardware_mailbox_read(p_self->p_mailbox[i], &mex))
    {
      shared_mex->data_buffer = mex.full_word;
      shared_mex->timestamp = timer_time_now();
    }
    atomic_store(&shared_mex->lock, 0);
  }
  return 0;
}

#define TEMPLATE_SHARED_MESSAGE_READ_UNPACK_CAN_X(can_num)\
int8_t shared_message_read_unpack_can##can_num (SharedMessageReader_h* const restrict self, can_obj_can##can_num##_h_t* o##can_num )\
{\
  const union SharedMessageReader_h_t_conv conv = {self};\
  const struct shared_message_reader_t* p_self = conv.clear;\
\
  uint64_t mex=0;\
  if(shared_message_read(self, &mex)>0)\
  {\
    const uint8_t mex_size = (uint8_t) message_dlc_can##can_num (can_id[p_self->m_id].can_id);\
    unpack_message_can##can_num (o##can_num , can_id[p_self->m_id].can_id, mex, mex_size, (unsigned int) p_self->m_last_read);\
    return 1;\
  }\
  return 0;\
}

TEMPLATE_SHARED_MESSAGE_READ_UNPACK_CAN_X(1)
TEMPLATE_SHARED_MESSAGE_READ_UNPACK_CAN_X(2)
TEMPLATE_SHARED_MESSAGE_READ_UNPACK_CAN_X(3)
