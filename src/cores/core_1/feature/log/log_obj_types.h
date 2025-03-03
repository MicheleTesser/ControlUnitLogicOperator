#ifndef __LOG_OBJ_TYPES__
#define __LOG_OBJ_TYPES__

#include <stdint.h>

typedef uint32_t DataRange;
typedef uint16_t DataPosition;

enum DATA_MODE{
    __u8__=0,
    __u16__,
    __u32__,

    __i8__,
    __i16__,
    __i32__,

    __float__,
};

#endif // !__LOG_OBJ_TYPES__
