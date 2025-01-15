#include "./fans.h"
#include "../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../board_can/board_can.h"
#include "../../board_conf/id_conf.h"
#include <stdint.h>
#include <string.h>

//private

typedef int8_t (*fan_setup_fun)(const uint8_t speed, const uint8_t enable);

struct Fan{
    float* fan_speed;
    fan_setup_fun set_f;
    uint8_t active: 1;
    uint8_t write_ptr: 1;
};

static struct{
    struct Fan fans[NUMBER_OF_FAN_TYPES];
    uint8_t init_done:1;
}FANS;

static int8_t setup_fan_radiator(const uint8_t fan_speed, const uint8_t enable)
{
    can_obj_can2_h_t o;
    CanMessage mex;
    memset(&o, 0, sizeof(o));
    memset(&mex, 0, sizeof(mex));
    o.can_0x130_Pcu.fan_enable = enable;
    o.can_0x130_Pcu.fan_speed = fan_speed;
    mex.id = CAN_ID_PCU;
    mex.message_size = pack_message_can2(&o, CAN_ID_PCU, &mex.full_word);
    return board_can_write(CAN_MODULE_GENERAL, &mex);
}

//public
int8_t fan_init(void)
{
    if (!FANS.init_done) {
        FANS.fans[0].set_f = setup_fan_radiator;
       FANS.init_done =1;
    }
    return 0;
}

struct Fan* fan_get_mut(const enum FAN_TYPES type)
{
    struct Fan* res = &FANS.fans[type];
    while (!FANS.init_done || res->write_ptr) {}
    res->write_ptr++;

    return res;

}

int8_t fan_enable(struct Fan* const restrict self)
{
    self->active = 1;
    if (self->set_f) {
        return self->set_f(0,1);
    }
    return 0;
}

int8_t fan_disable(struct Fan* const restrict self)
{
    self->active = 0;
    if (self->set_f) {
        return self->set_f(0,0);
    }
    return 0;
}

int8_t fan_set_speed(struct Fan* const restrict self, const float value)
{
    *self->fan_speed = value;
    if (self->set_f) {
        return self->set_f(*self->fan_speed, self->active);
    }
    return 0;
}
