#ifndef __AMK_POWER_SYSTEM__
#define __AMK_POWER_SYSTEM__

#include <stdint.h>
#include "../engine_common.h"

//INFO: doc/amk_datasheet.pdf page 61

struct AmkEngine_h{
    uint8_t private_data[14];
};

int8_t amk_module_init(struct AmkEngine_h* const restrict self, const enum ENGINES engine);
int8_t amk_update_status(struct AmkEngine_h* const restrict self);

#define EngineType struct AmkEngine_h

#define engine_module_init(engine_ptr, engine) amk_module_init(engine_ptr, engine)


#endif // !__AMK_POWER_SYSTEM__

