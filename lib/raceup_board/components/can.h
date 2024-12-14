#ifndef __VIRTUAL_CAN__
#define __VIRTUAL_CAN__

#include <stdint.h>
#include "./common_idx/common_idx.h"

typedef struct{
    uint32_t id;
    union{
        uint8_t buffer[8];
        uint32_t words[2];
        uint64_t full_word;
    };
    uint8_t message_size;
}CanMessage;

extern int8_t hardware_init_can(const BoardComponentId id,uint32_t baud_rate);
extern int8_t hardware_read_can(const BoardComponentId id, CanMessage* mex);
extern int8_t hardware_write_can(const BoardComponentId id, const CanMessage* restrict const mex);

#endif // !__VIRTUAL_CAN__
