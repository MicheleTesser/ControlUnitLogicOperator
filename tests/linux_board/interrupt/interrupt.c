#include "interrupt.h"

#include <signal.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

static struct{
    interrupt_fun interrupt_table[1024];
    uint8_t interrupt_enabled;
}interrupt_info;

//private
static void interrupt_fun_handler(int signal __attribute_maybe_unused__,siginfo_t* info, 
        void* args __attribute_maybe_unused__){
    interrupt_fun fun = interrupt_info.interrupt_table[info->si_value.sival_int];
    if (fun && interrupt_info.interrupt_enabled) {
        fun();
    }
}

//public
int8_t hardware_init_interrupt(void)
{
    struct sigaction act;
    act.sa_sigaction = interrupt_fun_handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);
    hardware_interrupt_enable();

    return 0;
}

int8_t hardware_interrupt_attach_fun(const BoardComponentId fun_id,
        const interrupt_fun fun)
{
    interrupt_info.interrupt_table[fun_id] = fun;
    return 0;
}

void raise_interrupt(uint8_t interrupt_number)
{
    union sigval value;

    value.sival_int = interrupt_number;
    sigqueue(getpid(), SIGUSR1, value);

}

int8_t hardware_interrupt_enable(void)
{
    interrupt_info.interrupt_enabled = 1;
    return 0;
}

int8_t hardware_interrupt_disable(void)
{
    interrupt_info.interrupt_enabled =0;
    return 0;
}
