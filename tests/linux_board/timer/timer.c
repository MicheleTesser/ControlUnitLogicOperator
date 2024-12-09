#include "./raceup_board/raceup_board.h"
#include <unistd.h>

int8_t wait_milliseconds(const uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
    return 0;
}
