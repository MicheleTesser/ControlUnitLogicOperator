#include "giei.h"
#include "engines/engine_common.h"
#include "engines/engines.h"
#include <stdint.h>

struct Giei_t{
    EngineType inverter;
};

union Giei_conv{
    struct Giei_h* hidden;
    struct Giei_t* clear;
};

const uint8_t giei_size_check[(sizeof(struct Giei_h) == sizeof(struct Giei_t))? 1 : -1];
#ifdef DEBUG
#endif /* ifdef DEBUG */

int8_t giei_init(struct Giei_h* const restrict self)
{
    union Giei_conv g_conv = {self};
    struct Giei_t* p_self = g_conv.clear;
    inverter_module_init(&p_self->inverter);

    return 0;
}
enum RUNNING_STATUS GIEI_check_running_condition(struct Giei_h* const restrict self);

