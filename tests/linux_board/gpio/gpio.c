#include "./gpio.h"
#include <stdatomic.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <gpiod.h>
#include <stdint.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>

#ifndef CHIP_PATH_BASIC
#define CHIP_PATH_BASIC "/dev/gpiochip1"
#define CHIP_PATH_PWM "/dev/gpiochip2"
#endif /* ifndef CHIP_PATH_BASIC */

#define gpio_pin_cnt (50)
static struct {
  struct gpiod_line_request* line_request;
  enum gpiod_line_value init_values[gpio_pin_cnt];
  atomic_bool taken[gpio_pin_cnt];
}lines;

#define PWM_LINE_SIZE (16)
#define gpio_pin_cnt_pwm (50)
static struct {
  struct gpiod_line_request* line_request;
  enum gpiod_line_value init_values[gpio_pin_cnt_pwm];
  atomic_bool taken[gpio_pin_cnt_pwm];
}lines_pwm;

struct __attribute__((aligned(4))) GpioRead_t{
  uint16_t gpio_id;
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
uint8_t __static_size_check_gpio_mut[(sizeof(Gpio_h) == sizeof(struct Gpio_t))?1:-1];

uint8_t __static_align_check_gpio_read[(_Alignof(GpioRead_h) == _Alignof(struct GpioRead_t))?1:-1];
uint8_t __static_align_check_gpio_mut[(_Alignof(Gpio_h) == _Alignof(struct Gpio_t))?1:-1];
#endif /* ifdef DEBUG*/

int8_t _assign_line(uint16_t* p_store_id, const enum GPIO_PIN line)
{
  if(atomic_load(&lines.taken[line]))
  {
    return -1;
  }
  atomic_store(&lines.taken[line], 1); 
  *p_store_id=line;
  return 0;
}

int8_t _assign_line_pwm(uint16_t* p_store_id, const enum GPIO_PWM_PIN line)
{
  if(atomic_load(&lines_pwm.taken[line]))
  {
    return -1;
  }
  atomic_store(&lines_pwm.taken[line], 1); 
  *p_store_id=line;
  return 0;
}

int8_t _create_virtual_chip_pwm(void)
{
  int8_t err =0;
  uint32_t line_offsets[gpio_pin_cnt_pwm];

  lines_pwm.line_request = NULL;
  for (int i =0; i<gpio_pin_cnt_pwm; i++) {
    lines_pwm.init_values[i] = GPIOD_LINE_VALUE_INACTIVE;
  }

  static struct gpiod_chip* chip = NULL;
  struct gpiod_line_config* line_config = gpiod_line_config_new();
  struct gpiod_line_settings *settings = gpiod_line_settings_new();
  struct gpiod_request_config *request_config = gpiod_request_config_new();
  assert(line_config);
  assert(settings);
  assert(request_config);

  for (int i =0; i<gpio_pin_cnt_pwm; i++)
  {
    line_offsets[i] = i;
  }

  if(gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT) < 0){
    fprintf(stderr, "gpio failed settings direction pwm\n");
    goto error_setting_direction;
  }
  if(gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_PUSH_PULL) < 0){
    fprintf(stderr, "gpio failed settings drive pwm\n");
    goto error_setting_drive;
  }

  if(gpiod_line_config_add_line_settings(line_config, line_offsets, gpio_pin_cnt_pwm, settings)<0)
  {
    fprintf(stderr, "gpio failed setting line config settings pwm\n");
    goto error_add_line_settings;
  }


  if(gpiod_line_config_set_output_values(line_config, lines_pwm.init_values, gpio_pin_cnt_pwm) < 0){
    fprintf(stderr, "gpio init failed settings init value of lines pwm\n");
    goto error_set_init_values;
  }

  chip = gpiod_chip_open(CHIP_PATH_PWM);
  if (!chip) {
    fprintf(stderr, "failed opening gpio-mockup chip pwm\n");
    goto error_opening_chip;
  }

  gpiod_request_config_set_consumer(request_config, "output simple");

  lines_pwm.line_request = gpiod_chip_request_lines(chip, request_config, line_config);
  if (!lines_pwm.line_request) {
    fprintf(stderr, "failed chip request line pwm\n");
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


int8_t _create_virtual_chip_basic(void)
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
    fprintf(stderr, "gpio failed settings direction basic\n");
    goto error_setting_direction;
  }
  if(gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_PUSH_PULL) < 0){
    fprintf(stderr, "gpio failed settings drive basic\n");
    goto error_setting_drive;
  }

  if(gpiod_line_config_add_line_settings(line_config, line_offsets, gpio_pin_cnt, settings) < 0)
  {
    fprintf(stderr, "gpio failed setting line config settings basic\n");
    goto error_add_line_settings;
  }


  if(gpiod_line_config_set_output_values(line_config, lines.init_values, gpio_pin_cnt) < 0){
    fprintf(stderr, "gpio init failed settings init value of lines basic\n");
    goto error_set_init_values;
  }

  chip = gpiod_chip_open(CHIP_PATH_BASIC);
  if (!chip) {
    fprintf(stderr, "failed opening gpio-mockup chip basic\n");
    goto error_opening_chip;
  }

  gpiod_request_config_set_consumer(request_config, "output simple");

  lines.line_request = gpiod_chip_request_lines(chip, request_config, line_config);
  if (!lines.line_request) {
    fprintf(stderr, "failed chip request line basic\n");
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

//public

int8_t create_virtual_chip(void)
{
  return _create_virtual_chip_basic() | _create_virtual_chip_pwm();
}


int8_t hardware_init_read_permission_gpio(GpioRead_h* const restrict self,
    const uint16_t id)
{
  union GpioRead_h_t_conv conv = {self};
  struct GpioRead_t* p_self = conv.clear;
  p_self->gpio_id = id;
  return 0;
}

int8_t hardware_init_gpio(Gpio_h* const restrict self, const enum GPIO_PIN id)
{
  union Gpio_h_t_conv conv = {self};
  struct Gpio_t* const p_self = conv.clear;
  int8_t err=0;
  if((err=_assign_line(&p_self->read.gpio_id, id))<0)
  {
    return err;
  }
  gpio_set_high(self);
  return err;
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
  return (int8_t) r;
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
  return 0;
}

extern int8_t gpio_set_low(Gpio_h* const restrict self)
{
  struct Gpio_t* p_self = (struct Gpio_t*) self;
  const enum GPIO_PIN id = p_self->read.gpio_id;
  lines.init_values[id] =GPIOD_LINE_VALUE_ACTIVE;

  if(gpiod_line_request_set_values(lines.line_request,lines.init_values) <0){
    fprintf(stderr, "gpio %d, set low error\n",id); 
    return -1;
  }
  return 0;
}

//pwm


struct GpioPwm_t{
  struct Gpio_t gpio;
  thrd_t thread;
  const uint8_t duty_cycle;
  uint8_t duty_cycle_running:1;
  uint8_t only_reading:1;
};

union GpioPwm_h_t_conv{
  GpioPwm_h* const hidden;
  struct GpioPwm_t* const clear;
};

#ifdef DEBUG
uint8_t __static_size_check_gpio_pwm[(sizeof(GpioPwm_h) == sizeof(struct GpioPwm_t))?1:-1];
uint8_t __static_align_check_gpio_pwm[(_Alignof(GpioPwm_h) == _Alignof(struct GpioPwm_t))?1:-1];
#endif /* ifdef DEBUG */

//private

//public

extern int8_t hardware_init_gpio_pwm(GpioPwm_h* const restrict self , 
    const enum GPIO_PWM_PIN id)
{
  union GpioPwm_h_t_conv conv = {self};
  struct GpioPwm_t* const restrict p_self = conv.clear;

  return _assign_line_pwm(&p_self->gpio.read.gpio_id, id);
}

extern int8_t hardware_init_gpio_pwm_read_only(GpioPwm_h* const restrict self,
    const enum GPIO_PWM_PIN id)
{
  union GpioPwm_h_t_conv conv = {self};
  struct GpioPwm_t* const restrict p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));

  p_self->gpio.read.gpio_id=id;
  p_self->only_reading=1;

  return 0;
}

extern int8_t hardware_write_gpio_pwm(GpioPwm_h* const restrict self, const uint16_t duty_cycle)
{
  union GpioPwm_h_t_conv conv = {self};
  struct GpioPwm_t* const restrict p_self = conv.clear;

  if (!p_self->only_reading)
  {
    return -1;
  }

  for (uint8_t i=0; i<PWM_LINE_SIZE; i++)
  {
    uint8_t line_value = duty_cycle & (1<<i);
    lines_pwm.init_values[p_self->gpio.read.gpio_id + i] = line_value;
  }


  return 0;
}

extern uint16_t hardware_read_gpio_pwm(GpioPwm_h* const restrict self)
{
  union GpioPwm_h_t_conv conv = {self};
  struct GpioPwm_t* const restrict p_self = conv.clear;
  uint16_t res=0;

  for (uint8_t i=0; i<PWM_LINE_SIZE; i++)
  {
    res |= lines_pwm.init_values[p_self->gpio.read.gpio_id + i] << i;
  }

  return res;
}
