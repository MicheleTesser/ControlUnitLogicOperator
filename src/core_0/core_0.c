#include "core_0.h"
#include "../core_status/core_status.h"
#include "giei/giei.h"
#include <stdint.h>

//public

void main_0(void)
{

    //setup

    core_status_core_ready(CORE_0);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
    }
}
