#ifndef __DV_RES__
#define __DV_RES__

#include <stdint.h>

struct DvRes;

int8_t res_class_init(void);
const struct DvRes* res_class_get(void);
struct DvRes* res_class_get_mut(void);
int8_t res_check_go(const struct DvRes* self);
int8_t res_start_time(struct DvRes* self);

void res_free_read_ptr(void);
void res_free_write_ptr(void);

#define RES_READ_ONLY_ACTION(exp) \
{\
    const struct DvRes* res_read_ptr = res_class_get();\
    exp;\
    res_free_read_ptr();\
}

#define RES_MUT_ACTION(exp) \
{\
    struct DvRes* res_mut_ptr = res_class_get_mut();\
    exp;\
    res_free_write_ptr();\
}

#endif // !__DV_RES__
