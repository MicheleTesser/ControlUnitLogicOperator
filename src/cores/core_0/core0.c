#include "./core0.h"
#include "../../cooling/cooling.h"
#include "../../driver_input/driver_input.h"
#include "../../lib/DPS/dps_slave.h"
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
    hardware_init_trap();
    hardware_init_gpio(AIR_PRECHARGE_INIT);
    hardware_init_gpio(AIR_PRECHARGE_DONE);
    hardware_init_gpio(CORE_ALIVE_LED_1);
    hardware_init_gpio(SCS);
    hardware_init_serial(SERIAL);
    alive_fd = i_m_alive_init(300 MILLIS, CORE_ALIVE_LED_1);
    GIEI_initialize();
    pump_enable();
    fan_enable();


    //INFO: open the SCS to power off the HV if it is on. May happen when the lv restarts.
    gpio_set_low(SCS); 
    wait_milliseconds(100);
    gpio_set_high(SCS);
    while (dps_is_init_done()) {}
}

static void loop(void)
{
    const float throttle = driver_get_amount(THROTTLE);
    const float brake = driver_get_amount(BRAKE);
    const float regen = driver_get_amount(REGEN);

    i_m_alive(alive_fd);
    if (GIEI_check_running_condition() == RUNNING) {
        pump_init();
        fan_init();
        GIEI_input(throttle,brake,regen);
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
