#include "./core0.h"
#include "../core_status/core_status.h"
#include "../../cooling/pumps/pumps.h"
#include "../../cooling/fans/fans.h"
#include "../../driver_input/driver_input.h"
#include "../../missions/missons.h"
#include "../../lib/DPS/dps_slave.h"
#include "../../GIEI/giei.h"
#include "../../board_conf/id_conf.h"
#include "../../batteries/batteries.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../alive_blink/alive_blink.h"
#include <stdint.h>

//private

static alive_blink_fd alive_fd =0;

static void setup(void)
{
    while(hardware_init_serial(SERIAL) <0){
        serial_write_str(SERIAL, "serial init failed");
    };

    while(hardware_init_interrupt()<0){
        serial_write_str(SERIAL, "interrupt init failed");
    };
    while(hardware_init_trap() <0){
        serial_write_str(SERIAL, "trap init failed");
    };
    while(hardware_init_gpio(AIR_PRECHARGE_INIT) <0){
        serial_write_str(SERIAL, "air 1 gpio init failed");
    };
    while(hardware_init_gpio(AIR_PRECHARGE_DONE) <0){
        serial_write_str(SERIAL, "air 2 gpio init failed");
    };
    while(hardware_init_gpio(CORE_ALIVE_LED_1) <0){
        serial_write_str(SERIAL, "core 0 alive led gpio init failed");
    };
    while(hardware_init_gpio(SCS) <0){
        serial_write_str(SERIAL, "scs gpio init failed");
    };
    alive_fd = i_m_alive_init(300 MILLIS, CORE_ALIVE_LED_1);
    while(lv_init() <0){
        serial_write_str(SERIAL, "lv init failed");
    };
    while(hv_init() <0){
        serial_write_str(SERIAL, "hv init failed");
    };
    while(GIEI_init() <0){
        serial_write_str(SERIAL, "GIEI init failed");
    };
    while (dps_is_init_done()) {}
    pump_enable();
    fan_enable(FANS_RADIATOR);


    //INFO: open the SCS to power off the HV if it is on. May happen when the lv restarts.
    gpio_set_low(SCS); 
    wait_milliseconds(100);
    gpio_set_high(SCS);

    serial_write_str(SERIAL, "core 0 init done");

}

static void loop(void)
{
    float throttle = 0;
    float regen = 0;

    DRIVER_INPUT_READ_ONLY_ACTION({
        throttle = driver_get_amount(driver_input_read_ptr, THROTTLE);
        regen = driver_get_amount(driver_input_read_ptr, REGEN);
    })

    i_m_alive(alive_fd);
    if (get_current_mission() != NONE) {
        if (GIEI_check_running_condition() == RUNNING) {
            pump_init();
            fan_init();
            GIEI_input(throttle,regen);
        }
    }
}

//public
//INFO: Main logic operator core
void main_0(void)
{
    core_update_status(0, CORE_INIT);
    setup();
    core_update_status(0, CORE_READY);

    while (!are_cores_in_sync()){}

    for (;;) {
        loop();
    }
}
