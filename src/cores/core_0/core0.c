#include "./core0.h"
#include "../../pumps/pumps.h"
#include "../../fans/fans.h"
#include "../../driver_input/driver_input.h"
#include "../../GIEI/giei.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../alive_blink/alive_blink.h"
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


    gpio_set_high(SCS); //INFO: open air for HV when the board start
}

static void loop(void)
{
    i_m_alive(alive_fd);
    if (GIEI_check_running_condition()) {
        GIEI_enable();
        pump_enable();
        fan_enable();

        if (!pump_init_done()){
            pump_init();
        }
        if (!fan_init_done()){
            fan_init();
        }
    }else{
        GIEI_disable();
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
