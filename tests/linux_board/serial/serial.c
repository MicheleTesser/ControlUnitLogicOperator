#include "./raceup_board/raceup_board.h"
#include <stdio.h>
#include <sys/cdefs.h>

int8_t hardware_init_serial(const BoardComponentId id __attribute_maybe_unused__)
{
    return 0;
}

int8_t serial_setup(const BoardComponentId id __attribute_maybe_unused__,
        const uint32_t freq __attribute_maybe_unused__)
{
    return 0;
}

int8_t serial_read(const BoardComponentId id __attribute_maybe_unused__, 
        uint8_t* restrict const o_buffer,
        const uint32_t buffer_size __attribute_maybe_unused__)
{
    fflush(stdin);
    fscanf(stdin, "%s\n",o_buffer); 
    getchar();
    fflush(stdin);
    return 0;
}

int8_t serial_write(const BoardComponentId id __attribute_maybe_unused__, uint8_t* const restrict o_buffer,
       const uint32_t buffer_size)
{
    o_buffer[buffer_size] = '\0';
    printf("%s\n",o_buffer);
    return 0;
}
