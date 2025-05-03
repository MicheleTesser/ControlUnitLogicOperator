#ifndef __AS_NODE__
#define __AS_NODE__

#include <stdint.h>
#include "../mission_reader/mission_reader.h"

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[4];
}AsNodeRead_h;

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[68];
}AsNode_h;



int8_t
as_node_init(AsNode_h* const restrict self,
    CarMissionReader_h* const restrict p_car_mission_reader)__attribute__((__nonnull__(1,2)));

int8_t
as_node_update(AsNode_h* const restrict self)__attribute__((__nonnull__(1)));

uint8_t
as_node_get_status(const AsNode_h* const restrict self)__attribute__((__nonnull__(1)));

int8_t
as_node_read_init(AsNodeRead_h* const restrict self)__attribute__((__nonnull__(1)));

uint8_t
as_node_read_get_status(const AsNodeRead_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__AS_NODE__
