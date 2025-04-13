#include "maps.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop
#include "../../../../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#include <string.h>

enum MAPS_TYPE{
    MAPS_TYPE_POWER=0,
    MAPS_TYPE_REGEN,
    MAPS_TYPE_TV_REPARTITION,
};

struct PowerMap{
  struct{
    float power_kw;
    float torque_pos;
  }map_list[10];
  uint8_t active;
};

struct RegenMap{
  struct{
    float regen_scale;
    float max_neg_torque;
  }map_list[10];
  uint8_t active;
};

struct TvRepartitionMap{
  struct{
    float repartition;
    uint8_t tv_active:1;
  }map_list[10];
  uint8_t active;
};

struct DrivingMaps_t{
  struct PowerMap power_map;
  struct RegenMap regen_map;
  struct TvRepartitionMap tv_repartition_map;
  struct CanMailbox* map_mailbox;
};

union DrivingMaps_h_t_conv{
  DrivingMaps_h* const restrict hidden;
  struct DrivingMaps_t* const restrict clear;
};

union DrivingMaps_h_t_conv_const{
  const DrivingMaps_h* const restrict hidden;
  const struct DrivingMaps_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_core_0_maps[(sizeof(DrivingMaps_h)) == sizeof(struct DrivingMaps_t)? 1:-1];
char __assert_align_core_0_maps[(_Alignof(DrivingMaps_h)) == _Alignof(struct DrivingMaps_t)? 1:-1];
#endif // DEBUG

static inline void _set_pow_map(struct DrivingMaps_t* const restrict self,
    const uint8_t map_i, const float kw, const float torque)
{
  struct PowerMap* map = &self->power_map;
  map->map_list[map_i].power_kw=kw;
  map->map_list[map_i].torque_pos =torque;
}

static inline void _set_regen_map(struct DrivingMaps_t* const restrict self,
    const uint8_t map_i, const float regen_scale, const float torque)
{
  struct RegenMap* map = &self->regen_map;
  map->map_list[map_i].regen_scale = regen_scale;
  map->map_list[map_i].max_neg_torque = torque;
}

static inline void _set_repartition_map(struct DrivingMaps_t* const restrict self,
    const uint8_t map_i, const float repartition, const uint8_t tv)
{
  struct TvRepartitionMap* map = &self->tv_repartition_map;
  map->map_list[map_i].repartition = repartition;
  map->map_list[map_i].tv_active = tv;
}


static void _init_power_maps(struct DrivingMaps_t* const restrict self)
{
  _set_pow_map(self, 0, 77, 21);
  _set_pow_map(self, 1, 75, 20);
  _set_pow_map(self, 2, 70, 18);
  _set_pow_map(self, 3, 60, 16);
  _set_pow_map(self, 4, 50, 15);
  _set_pow_map(self, 5, 40, 15);
  _set_pow_map(self, 6, 35, 13);
  _set_pow_map(self, 7, 30, 13);
  _set_pow_map(self, 8, 15, 12);
  _set_pow_map(self, 9, 10, 10);
}

static void _init_regen_maps(struct DrivingMaps_t* const restrict self)
{
  _set_regen_map(self, 0, 0, 0);
  _set_regen_map(self, 1, 20, -8);
  _set_regen_map(self, 2, 30, -10);
  _set_regen_map(self, 3, 40, -12);
  _set_regen_map(self, 4, 50, -15);
  _set_regen_map(self, 5, 60, -17);
  _set_regen_map(self, 6, 70, -18);
  _set_regen_map(self, 7, 80, -19);
  _set_regen_map(self, 8, 90, -20);
  _set_regen_map(self, 9, 100, -21);
}

static void _init_repartition_maps(struct DrivingMaps_t* const restrict self)
{
  _set_repartition_map(self, 0, 0.50f, 1);
  _set_repartition_map(self, 1, 1.0f, 0);
  _set_repartition_map(self, 2, 0.82f, 0);
  _set_repartition_map(self, 3, 0.80f, 0);
  _set_repartition_map(self, 4, 0.78f, 0);
  _set_repartition_map(self, 5, 0.75f, 0);
  _set_repartition_map(self, 6, 0.70f, 0);
  _set_repartition_map(self, 7, 0.60f, 0);
  _set_repartition_map(self, 8, 0.50f, 0);
  _set_repartition_map(self, 9, 0.50f, 0);
}

//public

int8_t driving_maps_init(DrivingMaps_h* const restrict self)
{
  union DrivingMaps_h_t_conv conv = {self};
  struct DrivingMaps_t* const restrict p_self = conv.clear;
  struct CanNode* can_node = NULL;

  memset(p_self, 0, sizeof(*p_self));

  ACTION_ON_CAN_NODE(CAN_GENERAL,can_node)
  {
    p_self->map_mailbox =
      hardware_get_mailbox_single_mex(
          can_node,
          RECV_MAILBOX,
          CAN_ID_MAP,
          message_dlc_can2(CAN_ID_MAP));
  }

  if (!p_self->map_mailbox)
  {
    return -1;
  }

  _init_power_maps(p_self);
  _init_regen_maps(p_self);
  _init_repartition_maps(p_self);

  return 0;
}

int8_t driving_map_update(DrivingMaps_h* const restrict self)
{
  union DrivingMaps_h_t_conv conv = {self};
  struct DrivingMaps_t* const restrict p_self = conv.clear;
  can_obj_can2_h_t o;
  CanMessage mex;
  if(hardware_mailbox_read(p_self->map_mailbox,&mex))
  {
    unpack_message_can2(&o, CAN_ID_MAP, mex.full_word, mex.message_size, timer_time_now());
    p_self->power_map.active = o.can_0x064_Map.power;
    p_self->regen_map.active = o.can_0x064_Map.regen;
    p_self->tv_repartition_map.active = o.can_0x064_Map.torque_rep;
  }

  return 0;
}

float driving_map_get_parameter(const DrivingMaps_h* const restrict self,
    const enum CAR_PARAMETERS param)
{
  union DrivingMaps_h_t_conv_const conv = {self};
  const struct DrivingMaps_t* const restrict p_self = conv.clear;
  const struct PowerMap* power_map =NULL;
  const struct RegenMap* regen_map =NULL;
  const struct TvRepartitionMap* tv_repartition =NULL;

  switch (param)
  {
    case MAX_POS_TORQUE:
      power_map = &p_self->power_map;
      return power_map->map_list[power_map->active].torque_pos;
    case POWER_KW:
      power_map = &p_self->power_map;
      return power_map->map_list[power_map->active].power_kw;
    case MAX_NEG_TORQUE:
      regen_map = &p_self->regen_map;
      return regen_map->map_list[regen_map->active].max_neg_torque;
    case REGEN_SCALE:
      regen_map = &p_self->regen_map;
      return regen_map->map_list[regen_map->active].regen_scale;
    case TV_ON:
      tv_repartition = &p_self->tv_repartition_map;
      return tv_repartition->map_list[tv_repartition->active].tv_active;
    case TORQUE_REPARTITION:
      tv_repartition = &p_self->tv_repartition_map;
      return tv_repartition->map_list[tv_repartition->active].repartition;
  }

  return -1;
}
