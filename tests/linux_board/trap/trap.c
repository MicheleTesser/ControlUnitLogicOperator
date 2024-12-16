#include "trap.h"
#include <stdatomic.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

#define MAX_TRAP 32

static struct{
    trap_fun trap_table[MAX_TRAP];
    _Atomic uint8_t trap_vec[MAX_TRAP];
    _Atomic uint8_t trap_enabled;
    uint8_t init_done : 1;
}trap_info;

//private
static inline void wait_init(void){
    while (!trap_info.init_done) {}
}

static void default_trap_fun(void){
    printf("empty trap\n");
}

static void* trap_dispatcher(void* args __attribute_maybe_unused__){
    for(uint8_t i=0;;i = (i+1)%MAX_TRAP){
        uint8_t trap_num = trap_info.trap_vec[i];
        if (trap_info.trap_enabled && trap_num)
        {
            if (trap_num < MAX_TRAP) {
                trap_info.trap_table[trap_num]();
                trap_info.trap_vec[i] = 0;
            }else{
                fprintf(stderr, "invalid trap %d\n", trap_num);
            }
        }
    }

    return NULL;
}

//public
int8_t hardware_init_trap(void)
{
    trap_info.init_done = 0;
    pthread_t trap_dispatch;
    memset(trap_info.trap_vec, 0, sizeof(trap_info.trap_vec[0]));
    for (int i =0; i<MAX_TRAP; i++) {
        trap_info.trap_table[i] = default_trap_fun;
    }
    pthread_create(&trap_dispatch, NULL, trap_dispatcher, NULL);

    trap_info.init_done = 1;
    hardware_trap_enable();
    return 0;
}

int8_t hardware_trap_attach_fun(volatile const BoardComponentId fun_id,
        volatile const trap_fun fun)
{
    wait_init();
    trap_info.trap_table[fun_id] = fun;
    return 0;
}

void raise_trap(volatile uint8_t trap_number)
{
    wait_init();
    trap_info.trap_vec[trap_number] = 1;
}

int8_t hardware_trap_enable(void)
{
    wait_init();
    trap_info.trap_enabled = 1;
    return 0;
}

int8_t hardware_trap_disable(void)
{
    wait_init();
    trap_info.trap_enabled =0;
    return 0;
}
