#include "core_2.h"
#include "../core_status/core_status.h"
#include <stdint.h>

//public

void main_2(void)
{

    //setup

    core_status_core_ready(CORE_2);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
    }
}
