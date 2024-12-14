#include "giei.h"
#include "./amk/amk.h"
#include "../driver_input/driver_input.h"
#include "../board_conf/id_conf.h"
#include "../lib/raceup_board/raceup_board.h"
#include <stdio.h>

static struct{
    time_var_microseconds sound_start_at;
    uint8_t running;
}GIEI;

//private
static uint8_t GIEI_get_hv_status(void)
{
    return 
        inverter_hv_status() &&
        gpio_read_state(AIR_PRECHARGE_INIT) && 
        gpio_read_state(AIR_PRECHARGE_DONE);
}

//public

uint8_t GIEI_check_running_condition(void)
{
    const uint8_t brake_treshold_percentage = 10;
    const time_var_microseconds sound_duration = 3 * 1000 * 1000;

    if ((timer_time_now() - GIEI.sound_start_at) > sound_duration) {
        gpio_set_high(READY_TO_DRIVE_OUT_SOUND);
    }
    if (!GIEI.running && GIEI_get_hv_status()) 
    {
        //starting
        if (driver_get_amount(BRAKE) > brake_treshold_percentage && 
                gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON))
        {
            //starting ok
            if (gpio_read_state(AIR_PRECHARGE_INIT) && gpio_read_state(AIR_PRECHARGE_DONE)) 
            {
                gpio_set_low(READY_TO_DRIVE_OUT_LED);
                gpio_set_low(READY_TO_DRIVE_OUT_SOUND);
                GIEI.sound_start_at = timer_time_now();
                GIEI.running =1;
            //starting failed. Precharge not finished. opening scs to stop precharge
            }else if(!gpio_read_state(AIR_PRECHARGE_INIT) || !gpio_read_state(AIR_PRECHARGE_DONE)){
                gpio_set_low(SCS);
            }
        //reset scs. can start again the precharge
        }else if(!GIEI_get_hv_status()){
            gpio_set_high(SCS);
        }
    //exiting from R2D
    }else if (GIEI.running && (!GIEI_get_hv_status() || !gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON))) 
    {
        GIEI.running =0;
        gpio_set_high(READY_TO_DRIVE_OUT_LED);
        stop_engines();
    }
    if (GIEI.running) {
        printf("running\n");
    }
    //continue with what you were doing
    return GIEI.running;
}

int8_t GIEI_recv_data(const CanMessage* const restrict mex)
{
    update_status(mex);
    return 0;
}
