#include "as_node.h"
#include "../mission_reader/mission_reader.h"
#include "../../../lib/raceup_board/raceup_board.h"

#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

struct AsNodeRead_t{
  GpioRead_h m_gpio_as_node;
};

struct AsNode_t{
  CarMissionReader_h* p_mission_reader;
  Gpio_h m_gpio_as_node;
};


union AsNode_h_t_conv{
  AsNode_h* const hidden;
  struct AsNode_t* const clear;
};

union AsNode_h_t_conv_const{
  const AsNode_h* const hidden;
  const struct AsNode_t* const clear;
};

union AsNodeRead_h_t_conv{
  AsNodeRead_h* const hidden;
  struct AsNodeRead_t* const clear;
};

union AsNodeRead_h_t_conv_const{
  const AsNodeRead_h* const hidden;
  const struct AsNodeRead_t* const clear;
};

#ifdef DEBUG
char __assert_size_as_node[(sizeof(AsNode_h)==sizeof(struct AsNode_t))?+1:-1];
char __assert_align_as_node[(_Alignof(AsNode_h)==_Alignof(struct AsNode_t))?+1:-1];

char __assert_size_as_node[(sizeof(AsNodeRead_h)==sizeof(struct AsNodeRead_t))?+1:-1];
char __assert_align_as_node[(_Alignof(AsNodeRead_h)==_Alignof(struct AsNodeRead_t))?+1:-1];
#endif /* ifdef DEBUG */

//private

static atomic_bool AS_NODE_OWNING =0;

static uint8_t _check_embedded(struct AsNode_t *const restrict self __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return 0;
}

static uint8_t _check_ebs(struct AsNode_t *const restrict self __attribute__((__unused__)))
{
  //TODO: not yet implemented
  return 0;
}

static inline uint8_t _as_node_enable(struct AsNode_t* const restrict self)
{
  gpio_set_low(&self->m_gpio_as_node);
  return 1;
}

static inline uint8_t _as_node_disable(struct AsNode_t* const restrict self)
{
  gpio_set_high(&self->m_gpio_as_node);
  return 0;
}

//public

int8_t as_node_init(AsNode_h* const restrict self,
    CarMissionReader_h* const restrict p_car_mission_reader)
{
  union AsNode_h_t_conv conv = {self};
  struct AsNode_t* const p_self = conv.clear;

  atomic_bool expected_value =0;
  if (!atomic_compare_exchange_strong(&AS_NODE_OWNING, &expected_value, 1))
  {
    return -1;
  }

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_gpio(&p_self->m_gpio_as_node, GPIO_AS_NODE)<0)
  {
    atomic_store(&AS_NODE_OWNING, 0);
    return -2;
  }

  p_self->p_mission_reader = p_car_mission_reader;

  return 0;
}

int8_t as_node_update(AsNode_h* const restrict self)
{
  union AsNode_h_t_conv conv = {self};
  struct AsNode_t* const p_self = conv.clear;

  switch (car_mission_reader_get_current_mission(p_self->p_mission_reader))
  {
    case CAR_MISSIONS_HUMAN:
      if (!_check_ebs(p_self))
      {
        return _as_node_enable(p_self);
      }
      else
      {
        return _as_node_disable(p_self);
      }
    case CAR_MISSIONS_DV_SKIDPAD:
    case CAR_MISSIONS_DV_AUTOCROSS:
    case CAR_MISSIONS_DV_TRACKDRIVE:
    case CAR_MISSIONS_DV_EBS_TEST:
    case CAR_MISSIONS_DV_INSPECTION:
      if (_check_ebs(p_self) && _check_embedded(p_self))
      {
        return _as_node_enable(p_self);
      }
      else
      {
        return _as_node_disable(p_self);
      }
    default:
      return _as_node_disable(p_self);
  }
}

int8_t as_node_read_init(AsNodeRead_h* const restrict self)
{
  union AsNodeRead_h_t_conv conv = {self};
  struct AsNodeRead_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  if (hardware_init_read_permission_gpio(&p_self->m_gpio_as_node, GPIO_AS_NODE)<0)
  {
    return -1;
  }

  return 0;
}

uint8_t as_node_read_get_status(const AsNodeRead_h* const restrict self)
{
  const union AsNodeRead_h_t_conv_const conv = {self};
  const struct AsNodeRead_t* const p_self = conv.clear;

  return gpio_read_state(&p_self->m_gpio_as_node);
}
