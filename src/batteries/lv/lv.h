#ifndef __LV_BATTERIE__
#define __LV_BATTERIE__

#include "../../lib/raceup_board/components/can.h"
#include <stdint.h>

struct Lv;

int8_t lv_init(void);
struct Lv* lv_get_mut(void);
int8_t lv_update_status(struct Lv* const restrict self, const CanMessage * const restrict mex);

void lv_free_mut_ptr(void);


#define LV_MUT_ACTION(exp)\
{\
    struct Lv* lv_mut_ptr = lv_get_mut();\
    exp;\
    lv_free_mut_ptr();\
}

#endif // !__LV_BATTERIE__
