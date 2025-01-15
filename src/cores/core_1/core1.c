#include "./core1.h"
#include "../core_status/core_status.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../../missions/missons.h"
#include "../../board_conf/id_conf.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../alive_blink/alive_blink.h"
#include "../../board_can/board_can.h"
#include "../../car_status/car_status.h"

static alive_blink_fd alive_fd;

static void setup(void)
{
    while(hardware_init_gpio(CORE_ALIVE_LED_2)<0){
        serial_write_str(SERIAL, "core 1 alive led gpio init failed");
    };
    while(board_can_init(CAN_MODULE_INVERTER, _1_MBYTE_S_)<0){
        serial_write_str(SERIAL, "can inverter init failed");
    };
    while(board_can_init(CAN_MODULE_GENERAL, _500_KBYTE_S_)<0){
        serial_write_str(SERIAL, "can general init failed");
    };
    while(board_can_init(CAN_MODULE_DV, _500_KBYTE_S_)<0){
        serial_write_str(SERIAL, "can dv init failed");
    };
    alive_fd = i_m_alive_init(100 MILLIS, CORE_ALIVE_LED_2);

    serial_write_str(SERIAL, "core 1 init done");

}

static void loop(void)
{
    i_m_alive(alive_fd);
    CanMessage mex;
    int8_t read_ok=-1;
    const time_var_microseconds read_time = timer_time_now();
    time_var_microseconds car_status_last_time = 0;

    read_ok = board_can_read(CAN_MODULE_INVERTER, &mex);
    if(read_ok >= 0){
        board_can_manage_message(CAN_MODULE_INVERTER, &mex, read_time);
    }

    read_ok = board_can_read(CAN_MODULE_GENERAL, &mex);
    if(read_ok >= 0){
        board_can_manage_message(CAN_MODULE_GENERAL, &mex, read_time);
    }

    if (get_current_mission() <= MANUALY) {
        read_ok = board_can_read(CAN_MODULE_DV, &mex);
        if(read_ok >= 0){
            board_can_manage_message(CAN_MODULE_DV, &mex, read_time);
        }
    }
    #ifndef DISABLE_FREQ_CONSISTENCY_CHECK
    board_can_consistency_check();
    #endif /* ifndef DISABLE_FREQ_CONSISTENCY_CHECK */

    if((timer_time_now() - car_status_last_time > 200 MILLIS) ){
        car_status_last_time = timer_time_now();
        car_status_send_status();
    }
}

//INFO: Service core
void main_1(void)
{
    core_update_status(1, CORE_INIT);
    setup();
    core_update_status(1, CORE_READY);

    while (!are_cores_in_sync()){}

    for(;;){
        loop();
    }
}
