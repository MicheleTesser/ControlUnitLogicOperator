#include <stdint.h>
#include "input_rtd.h"
#include "../../lib/raceup_board/raceup_board.h"
#include "../../board_conf/id_conf.h"


static struct{
    enum RTD_MODE mode;
}RTD;

int8_t input_rtd_class_init(void)
{
    RTD.mode = BUTTON;
    return 0;
}

int8_t input_rtd_set_mode(const enum RTD_MODE mode)
{
    RTD.mode = mode;
    return 0;
}

int8_t input_rtd_check(void)
{
    switch (RTD.mode) {
        case BUTTON:
            return gpio_read_state(READY_TO_DRIVE_INPUT_BUTTON);
        case RES:
            //TODO: GO button in RES for DV goes into input_rtd
            break;
    }
    return -1;
}
