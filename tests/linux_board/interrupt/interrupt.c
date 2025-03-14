#include "interrupt.h"
#include <stdatomic.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

#define MAX_INTERRUPTS 32u

static struct{
    interrupt_fun interrupt_table[MAX_INTERRUPTS];
    _Atomic uint8_t interr;
    _Atomic uint8_t interrupt_enabled;
    uint8_t init_done : 1;
}interrupt_info;

//private
static inline void wait_init(void){
    while (!interrupt_info.init_done) {
        printf("waiting init interrupt\n");
    }
}

static inline void default_interr_fun(void){
    printf("empty interrupt\n");
}

static void* interrupt_dispatcher(void* args __attribute_maybe_unused__){
    for(uint8_t i=0;;i = (i+1u)%MAX_INTERRUPTS){
        uint8_t interrup_num = interrupt_info.interr & (1 << i);
        if (interrupt_info.interrupt_enabled && interrup_num)
        {
            if (interrup_num < MAX_INTERRUPTS) {
                interrupt_info.interrupt_table[interrup_num]();
                interrupt_info.interr ^= interrup_num;
            }else{
                fprintf(stderr, "invalid interrupt %d\n", interrup_num);
            }
        }
    }

    return NULL;
}

//public
int8_t hardware_init_interrupt(void)
{
    if (interrupt_info.init_done) {
        return 0;
    }
    interrupt_info.init_done = 0;
    pthread_t interrupt_dispatch;
    interrupt_info.interr = 0;
    for (uint8_t i =0; i<MAX_INTERRUPTS; i++) {
        interrupt_info.interrupt_table[i] = default_interr_fun;
    }
    pthread_create(&interrupt_dispatch, NULL, interrupt_dispatcher, NULL);

    interrupt_info.init_done = 1;
    hardware_interrupt_enable();
    return 0;
}

int8_t hardware_interrupt_attach_fun(const enum INTERRUPT_SLOT interrupt_id,
        const interrupt_fun fun)
{
    if (interrupt_id == __NUM_OF_INTERRUPT__)
    {
        return -1;
    }
    wait_init();
    interrupt_info.interrupt_table[interrupt_id] = fun;
    return 0;
}

void raise_interrupt(const uint8_t interrupt_number)
{
    wait_init();
    interrupt_info.interr |= ((interrupt_number > 0) << (interrupt_number -1))
        + !interrupt_number;
}

int8_t hardware_interrupt_enable(void)
{
    wait_init();
    interrupt_info.interrupt_enabled = 1;
    return 0;
}

int8_t hardware_interrupt_disable(void)
{
    wait_init();
    interrupt_info.interrupt_enabled =0;
    return 0;
}
