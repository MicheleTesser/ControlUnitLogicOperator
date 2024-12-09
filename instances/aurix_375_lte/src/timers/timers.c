#include "IfxPort.h"
#include "Bsp.h"
#include "../../Libraries/ControlUnitLogicOperator/lib/raceup_board/components/timer.h"

int8_t hardware_init_timer(const BoardComponentId id)
{
    return 0;
}
extern int8_t wait_milliseconds(const BoardComponentId id, uint32_t time)
{
    switch (id) {
        case 0:
            waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, time));
            break;
        default:
            return -1;
    }
     return 0;
}
