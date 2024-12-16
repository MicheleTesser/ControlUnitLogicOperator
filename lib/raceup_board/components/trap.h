#ifndef __VIRTUAL_TRAP__
#define __VIRTUAL_TRAP__

#include <stdint.h>
#include "./common_idx/common_idx.h"

typedef void (*trap_fun) (void);

extern int8_t hardware_init_trap(void);
extern int8_t hardware_trap_enable(void);
extern int8_t hardware_trap_disable(void);
extern int8_t hardware_trap_attach_fun(const BoardComponentId fun_id,
        const trap_fun fun);

#endif // !__VIRTUAL_TRAP__
