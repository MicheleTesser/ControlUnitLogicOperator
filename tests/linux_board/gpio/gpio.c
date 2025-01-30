#include "./gpio.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <gpiod.h>
#include <stdint.h>
#include <sys/cdefs.h>

#ifndef CHIP_PATH
#define CHIP_PATH "/dev/gpiochip1"
#endif /* ifndef CHIP_PATH */

#define gpio_pin_cnt 50
static struct {
    struct gpiod_line_request* line_request;
    enum gpiod_line_value init_values[gpio_pin_cnt];
}lines;

struct GpioRead_t{
    enum GPIO_PIN gpio_id;
};

union GpioRead_h_t_conv{
  GpioRead_h* const restrict hidden;
  struct GpioRead_t* const restrict clear;
};

struct Gpio_t{
    struct GpioRead_t read;
    const uint8_t filler_data;
};

union Gpio_h_t_conv{
  Gpio_h* const restrict hidden;
  struct Gpio_t* const restrict clear;
};

#ifdef DEBUG

uint8_t __static_size_check_gpio_read[(sizeof(GpioRead_h) == sizeof(struct GpioRead_t))?1:-1];
uint8_t __static_size_check_gpio_read[(sizeof(Gpio_h) == sizeof(struct Gpio_t))?1:-1];

#endif /* ifdef DEBUG

uint8_t __static_size_check_gpio_read[(sizeof(GpioRead_h) == sizeof()]; */

//public

int8_t create_virtual_chip(void)
{
    int8_t err =0;
    uint32_t line_offsets[gpio_pin_cnt];

    lines.line_request = NULL;
    for (int i =0; i<gpio_pin_cnt; i++) {
        lines.init_values[i] = GPIOD_LINE_VALUE_INACTIVE;
    }

    static struct gpiod_chip* chip = NULL;
    struct gpiod_line_config* line_config = gpiod_line_config_new();
    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    struct gpiod_request_config *request_config = gpiod_request_config_new();
    assert(line_config);
    assert(settings);
    assert(request_config);

    for (int i =0; i<gpio_pin_cnt; i++) {
        line_offsets[i] = i;
    }
    if(gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT) < 0){
        fprintf(stderr, "gpio failed settings direction\n");
        goto error_setting_direction;
    }
    if(gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_PUSH_PULL) < 0){
        fprintf(stderr, "gpio failed settings drive\n");
        goto error_setting_drive;
    }

    if(gpiod_line_config_add_line_settings(line_config, line_offsets, gpio_pin_cnt, settings) < -1){
        fprintf(stderr, "gpio failed setting line config settings\n");
        goto error_add_line_settings;
    }


    if(gpiod_line_config_set_output_values(line_config, lines.init_values, gpio_pin_cnt) < 0){
        fprintf(stderr, "gpio init failed settings init value of lines\n");
        goto error_set_init_values;
    }

    chip = gpiod_chip_open(CHIP_PATH);
    if (!chip) {
        fprintf(stderr, "failed opening gpio-mockup chip\n");
        goto error_opening_chip;
    }

    gpiod_request_config_set_consumer(request_config, "output simple");

    lines.line_request = gpiod_chip_request_lines(chip, request_config, line_config);
    if (!lines.line_request) {
        fprintf(stderr, "failed chip request line\n");
        goto error_request_line;
    }

    gpiod_line_settings_free(settings);
    gpiod_request_config_free(request_config);
    gpiod_line_config_free(line_config);
    gpiod_chip_close(chip);
    settings = NULL;
    request_config = NULL;
    line_config = NULL;
    chip = NULL;

    return 0;

error_request_line:
    err--;
error_opening_chip:
    err--;
error_set_init_values:
    err--;
error_add_line_settings:
    err--;
error_setting_drive:
    err--;
error_setting_direction:
    if(settings) gpiod_line_settings_free(settings);
    if(request_config) gpiod_request_config_free(request_config);
    if(line_config) gpiod_line_config_free(line_config);
    if(chip) gpiod_chip_close(chip);
    settings = NULL;
    request_config = NULL;
    line_config = NULL;
    chip = NULL;
    err--;

    return err;
}

int8_t
hardware_init_read_permission_gpio(GpioRead_h* const restrict self,
        const enum GPIO_PIN id)
{
    struct GpioRead_t* p_self = (struct GpioRead_t*) self;
    p_self->gpio_id = id;
    return 0;
}

int8_t
hardware_init_gpio(Gpio_h* const restrict self , 
        const enum GPIO_PIN id)
{
    struct Gpio_t* p_self = (struct Gpio_t*) self;
    p_self->read.gpio_id=id;
    return 0;
}

int8_t gpio_toggle(Gpio_h* const restrict self)
{
    int8_t read = gpio_read_state(&self->gpio_read_permission);
    if (read >= 0) {
        if (read) {
            return gpio_set_high(self);
        }
        return gpio_set_low(self);
    }
    return -1;
}

int8_t gpio_read_state(const GpioRead_h* const restrict self)
{
    struct GpioRead_t * p_self = (struct GpioRead_t*) self;
    int r =gpiod_line_request_get_value(lines.line_request, p_self->gpio_id);
    if (r< 0) {
        fprintf(stderr, "gpio %d read err\n", p_self->gpio_id);
    }
    return r;
}

int8_t gpio_set_high(Gpio_h* const restrict self)
{
    struct Gpio_t* p_self = (struct Gpio_t*) self;
    lines.init_values[p_self->read.gpio_id] =GPIOD_LINE_VALUE_INACTIVE;
    const enum GPIO_PIN id = p_self->read.gpio_id;
    if(gpiod_line_request_set_values(lines.line_request,lines.init_values) <0){
        fprintf(stderr, "gpio %d, set high error\n",id); 
        return -1;
    }
    if (id == GPIO_SCS) {
        struct Gpio_t air1 = {.read.gpio_id = GPIO_AIR_PRECHARGE_INIT};
        gpio_set_high((struct Gpio_h*) &air1);
        struct Gpio_t air2 = {.read.gpio_id = GPIO_AIR_PRECHARGE_DONE};
        gpio_set_high((struct Gpio_h*) &air2);
    }
    return 0;
}

extern int8_t gpio_set_low(Gpio_h* const restrict self)
{
    struct Gpio_t* p_self = (struct Gpio_t*) self;
    const enum GPIO_PIN id = p_self->read.gpio_id;
    lines.init_values[id] =GPIOD_LINE_VALUE_ACTIVE;
    struct Gpio_t scs = {.read.gpio_id = GPIO_SCS};
    if (gpio_read_state((GpioRead_h*)&scs.read) &&
            (id == GPIO_AIR_PRECHARGE_INIT || id == GPIO_AIR_PRECHARGE_DONE)){
        return -2;
    }
    if(gpiod_line_request_set_values(lines.line_request,lines.init_values) <0){
        fprintf(stderr, "gpio %d, set low error\n",id); 
        return -1;
    }
    return 0;
}
