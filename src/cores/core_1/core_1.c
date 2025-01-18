#include "core_1.h"
#include "../core_status/core_status.h"
#include <stdint.h>

//public

void main_0(void)
{
    //setup

    core_status_core_ready(CORE_1);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
    }
}
