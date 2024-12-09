#include "./raceup_board/raceup_board.h"
#include <unistd.h>


int8_t hardware_init_timer(const BoardComponentId id __attribute_maybe_unused__){
    return 0;
}

int8_t wait_milliseconds(const BoardComponentId id __attribute_maybe_unused__, uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
    return 0;
}
