#include "interrupt.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

#define MAX_INTERRUPTS 1024

static struct{
    interrupt_fun interrupt_table[MAX_INTERRUPTS];
    uint8_t interrupt_enabled;
    uint64_t ev_fd;
}interrupt_info;

//private

static void default_interr_fun(void){
}

static void* interrupt_dispatcher(void* args __attribute_maybe_unused__){
    for(;;){
        uint64_t int_counter = 0;
        if (interrupt_info.interrupt_enabled &&
                read(interrupt_info.ev_fd, &int_counter, sizeof(int_counter)) > 0) 
        {
            if (int_counter < MAX_INTERRUPTS) {
                interrupt_info.interrupt_table[int_counter]();       
            }else{
                fprintf(stderr, "invalid interrupt %ld\n", int_counter);
            }
        }
    }

    return NULL;
}

//public
int8_t hardware_init_interrupt(void)
{
    pthread_t interrupt_dispatch;
    interrupt_info.ev_fd= eventfd(0, 0);
    if (interrupt_info.ev_fd < 0) {
        fprintf(stderr, "failed init event_fd\n");
        return -1;
    }
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

void raise_interrupt(uint64_t interrupt_number)
{
    write(interrupt_info.ev_fd, &interrupt_number, sizeof(interrupt_number));
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
