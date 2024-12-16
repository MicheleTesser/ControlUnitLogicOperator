#include "./core0.h"
#include "../../cooling/cooling.h"
#include "../../driver_input/driver_input.h"
#include "../../GIEI/giei.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../alive_blink/alive_blink.h"
#include <stdint.h>

//private

static alive_blink_fd alive_fd =0;

static void setup(void)
{
    hardware_init_interrupt();
    hardware_init_gpio(AIR_PRECHARGE_INIT);
    hardware_init_gpio(AIR_PRECHARGE_DONE);
    hardware_init_gpio(LED_1);
    hardware_init_gpio(SCS);
    hardware_init_serial(SERIAL);
    alive_fd = i_m_alive_init(300 MILLIS, LED_1);


    //INFO: open the SCS to power off the HV if it is on. May happen when the lv restarts.
    gpio_set_low(SCS); 
    wait_milliseconds(100);
    gpio_set_high(SCS);
}

static void loop(void)
{
    i_m_alive(alive_fd);
    if (GIEI_check_running_condition()) {
        pump_enable();
        fan_enable();

        if (!pump_init_done()){
            pump_init();
        }
        if (!fan_init_done()){
            fan_init();
        }
    }
}

//public
//INFO: Main logic operator core
void main_0(void)
{
    setup();
    for (;;) {
        loop();
    }
}
