#include "../../Libraries/ControlUnitLogicOperator/lib/raceup_board/components/timer/timer.h"
#include "IfxPort.h"
#include "Bsp.h"

int8_t hardware_init_timer(const uint16_t id)
{
    return 0;
}
extern int8_t wait_milliseconds(component_timer* const restrict comp, uint32_t time)
{
     waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, time));
     return 0;
}
