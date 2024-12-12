#include "../components/component.h"

#include <signal.h>
#include <stdint.h>

static interrupt_fun interrup_table[1024];

//private
static void interrupt_fun_handler(int id){
    interrup_table[id]();
}

//public
int8_t hardware_init_interrupt(void)
{
    signal(SIGUSR1, interrupt_fun_handler);
    return 0;
}

int8_t hardware_interrupt_attach_fun(const BoardComponentId fun_id,
        const interrupt_fun fun)
{
    interrup_table[fun_id] = fun;
    return 0;
}
