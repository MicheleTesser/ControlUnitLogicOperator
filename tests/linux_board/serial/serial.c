#include "./raceup_board/raceup_board.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

int8_t hardware_init_serial(void)
{
  return 0;
}

int8_t serial_setup(const uint32_t freq __attribute_maybe_unused__)
{
    return 0;
}

int8_t serial_read(uint8_t* restrict const o_buffer,
        const uint32_t buffer_size __attribute_maybe_unused__)
{
    fflush(stdin);
    fscanf(stdin, "%s\n",o_buffer); 
    getchar();
    fflush(stdin);
    return 0;
}

int8_t serial_write_str(const char* const restrict buffer)
{
    printf("%s\n",buffer);
    return 0;
}

int8_t serial_write_raw(const char* const restrict buffer)
{
    printf("%s",buffer);
    return 0;
}

#define TEMPLATE_SERIAL_WRITE_DIG(TYPE,FORM)\
int8_t serial_write_##TYPE (const TYPE dig)\
{\
    printf(FORM,dig);\
    return 0;\
}

TEMPLATE_SERIAL_WRITE_DIG(int8_t, "%d")
TEMPLATE_SERIAL_WRITE_DIG(int16_t, "%d")
TEMPLATE_SERIAL_WRITE_DIG(int32_t, "%d")

TEMPLATE_SERIAL_WRITE_DIG(uint8_t, "%d")
TEMPLATE_SERIAL_WRITE_DIG(uint16_t, "%d")
TEMPLATE_SERIAL_WRITE_DIG(uint32_t, "%d")

TEMPLATE_SERIAL_WRITE_DIG(float, "%f")
