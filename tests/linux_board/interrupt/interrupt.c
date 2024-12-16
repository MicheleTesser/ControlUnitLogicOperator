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
    _Atomic uint8_t interrupt_enabled;
    _Atomic uint8_t interr[MAX_INTERRUPTS];
}interrupt_info;

//private

static void default_interr_fun(void){
    printf("empty interrupt\n");
}

static void* interrupt_dispatcher(void* args __attribute_maybe_unused__){
    for(uint8_t i=0;;i = (i+1)%MAX_INTERRUPTS){
        uint8_t interrup_num = interrupt_info.interr[i];
        if (i==1) {
            // printf("checking interrupt %d with value %d\n",i,interrup_num);
        }
        if (interrupt_info.interrupt_enabled && interrup_num)
        {
            if (interrup_num < MAX_INTERRUPTS) {
                interrupt_info.interrupt_table[interrup_num]();
                interrupt_info.interr[i] = 0;
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
    pthread_t interrupt_dispatch;
    memset(interrupt_info.interr, 0, sizeof(interrupt_info.interr[0]));
    for (int i =0; i<MAX_INTERRUPTS; i++) {
        interrupt_info.interrupt_table[i] = default_interr_fun;
    }
    pthread_create(&interrupt_dispatch, NULL, interrupt_dispatcher, NULL);
    sleep(1);

    hardware_interrupt_enable();
    return 0;
}

int8_t hardware_interrupt_attach_fun(volatile const BoardComponentId fun_id,
        const interrupt_fun fun)
{
    interrupt_info.interrupt_table[fun_id] = fun;
    return 0;
}

void raise_interrupt(volatile uint8_t interrupt_number)
{
    interrupt_info.interr[interrupt_number] = 1;
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
