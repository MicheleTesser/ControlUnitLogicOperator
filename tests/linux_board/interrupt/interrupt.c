#include "interrupt.h"
#include <stdatomic.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

#define MAX_INTERRUPTS 32

static struct{
    interrupt_fun interrupt_table[MAX_INTERRUPTS];
    uint8_t interrupt_enabled;
    _Atomic int8_t interr;
}interrupt_info;

//private

static void default_interr_fun(void){
    printf("empty interrupt\n");
}

static void* interrupt_dispatcher(void* args __attribute_maybe_unused__){
    for(;;){
        if (interrupt_info.interrupt_enabled && interrupt_info.interr >=0)
        {
            if (interrupt_info.interr < MAX_INTERRUPTS) {
                interrupt_info.interrupt_table[interrupt_info.interr]();
                interrupt_info.interr = -1;
            }else{
                fprintf(stderr, "invalid interrupt %d\n", interrupt_info.interr);
            }
        }
    }

    return NULL;
}

//public
int8_t hardware_init_interrupt(void)
{
    pthread_t interrupt_dispatch;
    interrupt_info.interr =-1;
    for (int i =0; i<MAX_INTERRUPTS; i++) {
        interrupt_info.interrupt_table[i] = default_interr_fun;
    }
    pthread_create(&interrupt_dispatch, NULL, interrupt_dispatcher, NULL);
    sleep(1);

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
    interrupt_info.interr = interrupt_number;
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
