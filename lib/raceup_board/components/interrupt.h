#ifndef __VIRTUAL_INTERRUPT__
#define __VIRTUAL_INTERRUPT__

#include <stdint.h>
#include "./common_idx/common_idx.h"

typedef void (*interrupt_fun) (void);

extern int8_t hardware_init_interrupt(void);
extern int8_t hardware_interrupt_enable(void);
extern int8_t hardware_interrupt_disable(void);
extern int8_t hardware_interrupt_attach_fun(const BoardComponentId fun_id,
        const interrupt_fun fun);

#endif // !__VIRTUAL_INTERRUPT__
