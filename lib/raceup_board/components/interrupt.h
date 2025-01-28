#ifndef __VIRTUAL_INTERRUPT__
#define __VIRTUAL_INTERRUPT__

#include <stdint.h>

#ifndef INTERRUP_ATTRIBUTE
#define INTERRUP_ATTRIBUTE
#endif // !INTERRUP_ATTRIBUTE
typedef void (*interrupt_fun) (void);

enum INTERRUPT_SLOT{
    __NUM_OF_INTERRUPT__
};

extern int8_t hardware_init_interrupt(void);
extern int8_t hardware_interrupt_enable(void);
extern int8_t hardware_interrupt_disable(void);
extern int8_t hardware_interrupt_attach_fun(const enum INTERRUPT_SLOT interrupt_id,
        const interrupt_fun fun);

#endif // !__VIRTUAL_INTERRUPT__
