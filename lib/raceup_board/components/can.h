#ifndef __VIRTUAL_CAN__
#define __VIRTUAL_CAN__

#include <stdint.h>
#include "./common_idx/common_idx.h"

typedef struct{
    uint32_t id;
    void* buffer;
    uint8_t message_size;
}CanMessage;

extern int8_t hardware_init_can(const BoardComponentId id,uint32_t baud_rate);
extern int8_t hardware_read_can(const BoardComponentId id, uint32_t* msg_id,
        void* buffer, uint8_t* message_size);
extern int8_t hardware_write_can(const BoardComponentId id, const CanMessage* restrict const mex);

#endif // !__VIRTUAL_CAN__
