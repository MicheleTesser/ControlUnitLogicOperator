#ifndef __LOG_OBJ_TYPES__
#define __LOG_OBJ_TYPES__

#include <stdint.h>

typedef uint32_t DataRange;
typedef uint16_t DataPosition;

enum DATA_MODE{
    DATA_UNSIGNED=0,
    DATA_SIGNED,
    DATA_FLOATED,
};


#endif // !__LOG_OBJ_TYPES__
