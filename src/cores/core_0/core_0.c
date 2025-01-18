#include "core_0.h"
#include "../core_status/core_status.h"
#include "giei/giei.h"
#include <stdint.h>

//public

void main_0(void)
{
    //setup
    struct Giei_h giei;

    while (giei_init(&giei) <0) {}

    core_status_core_ready(CORE_0);
    while (core_status_ready_state()) {}

    //loop
    for(;;){
    }
}
