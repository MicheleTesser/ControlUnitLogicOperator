#ifndef __VIRTUAL_CAN__
#define __VIRTUAL_CAN__

#include <stdint.h>

typedef struct{
    uint32_t id;
    union{
        uint8_t buffer[8];
        uint32_t words[2];
        uint64_t full_word;
    };
    uint8_t message_size;
}CanMessage;

enum CAN_MODULES{
    CAN_INVERTER,
    CAN_GENERAL,
    CAN_DV,
};

enum CAN_FREQUENCY{
    _1_MBYTE_S_ = 1000000L,
    _500_KBYTE_S_ = 500000L,
    _250_KBYTE_S_ = 250000L,
    _125_KBYTE_S_ = 125000L,
    _50_KBYTE_S_ = 50000L,
    _20_KBYTE_S_ = 20000L,
    _10_KBYTE_S_ = 10000L,
    _5_KBYTE_S_ = 5000L,
};

extern int8_t hardware_init_can(const enum CAN_MODULES mod, const enum CAN_FREQUENCY baud_rate);
extern int8_t hardware_read_can(const enum CAN_MODULES mod, CanMessage* mex);
extern int8_t hardware_write_can(const enum CAN_MODULES mod, const CanMessage* restrict const mex);

#endif // !__VIRTUAL_CAN__
