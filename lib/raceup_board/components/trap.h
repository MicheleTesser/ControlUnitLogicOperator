#ifndef __VIRTUAL_TRAP__
#define __VIRTUAL_TRAP__

#include <stdint.h>
#include "./common_idx/common_idx.h"

#ifndef TRAP_ATTRIBUTE
#define TRAP_ATTRIBUTE
#endif // !TRAP_ATTRIBUTE
typedef void (*trap_fun) (void);

enum TRAP_SLOT{
    EMERGENCY_RAISED_TRAP,
    EMERGENCY_SOLVED_TRAP,
    TRAP_INPUT_RTD_TOGGLE,
};

extern int8_t hardware_init_trap(void);
extern int8_t hardware_trap_enable(void);
extern int8_t hardware_trap_disable(void);
extern int8_t hardware_trap_attach_fun(const BoardComponentId fun_id,
        const trap_fun fun);
extern void hardware_raise_trap(const uint8_t trap_number);

#endif // !__VIRTUAL_TRAP__
