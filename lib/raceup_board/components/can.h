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

enum CAN_MODULES{
    CAN_INVERTER,
    CAN_GENERAL,
    CAN_DV,
};

enum CAN_MAILBOXES_RECV{
    CORE_0_DRIVER_HUMAN,
    CORE_0_DRIVER_DV,
    CORE_0_IMU,
    CORE_0_HV,
    CORE_0_MAPS,

    CORE_1_IMU,
    CORE_1_DRIVER,
    CORE_1_REGEN,
    CORE_1_SUSP_FRONT,
    CORE_1_SUSP_REAR,

    CORE_2_IMU,
    CORE_2_DV_DRIVER_INPUT,
    CORE_2_HUMAN_DRIVER_INPUT,
};


//INFO: maintain the order
enum CAN_MAILBOXES_SEND{
    CORE_0_BMS_HV,

    CORE_1_FAN_PUMP,
};

struct CanNode;
struct CanMailbox;

struct CanNode*
hardware_init_can(const enum CAN_MODULES mod, const enum CAN_FREQUENCY baud_rate);

extern int8_t
hardware_read_can(struct CanNode* const restrict self __attribute__((__nonnull__)),
        CanMessage* const restrict mex __attribute__((__nonnull__)));

extern int8_t
hardware_write_can(struct CanNode* const restrict self __attribute__((__nonnull__)),
        const CanMessage* restrict const mex __attribute__((__nonnull__)));

extern int8_t
hardware_set_mailbox_can(struct CanNode* const restrict self __attribute__((__nonnull__)),
        const uint16_t id, const uint16_t mailbox);

extern struct CanMailbox*
hardware_get_mailbox(const enum CAN_MAILBOXES_RECV mailbox);

extern struct CanMailbox*
hardware_get_mailbox_send(const enum CAN_MAILBOXES_SEND mailbox);

extern int8_t
hardware_mailbox_read(const struct CanMailbox* const restrict self __attribute__((__nonnull__)),
        uint64_t* const restrict data __attribute__((__nonnull__)));

extern int8_t
hardware_mailbox_send(struct CanMailbox* const restrict self __attribute__((__nonnull__)),
        const uint64_t data);

extern void
hardware_free_mailbox_can(struct CanMailbox* const* const restrict self __attribute__((__nonnull__)));

#endif // !__VIRTUAL_CAN__
