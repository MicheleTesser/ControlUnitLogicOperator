#include "score_lib/test_lib.h"
#include "src/src.h"
#include "src/cores/core_utility/core_status/core_status.h"
#include "linux_board/linux_board.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>


int init_core_0(void* args __attribute_maybe_unused__){
  main_0();
  return 0;
}

int init_core_1(void* args __attribute_maybe_unused__){
  main_1();
  return 0;
}

int init_core_2(void* args __attribute_maybe_unused__){
  main_2();
  return 0;
}

int main(void)
{
  int8_t err=0;
  thrd_t core_0;
  thrd_t core_1;
  thrd_t core_2;
  GpioRead_h alive_led={0};

  if(create_virtual_chip() <0){
    err--;
    goto end;
  }


  thrd_create(&core_0, init_core_0, NULL);
  thrd_create(&core_1, init_core_1, NULL);
  thrd_create(&core_2, init_core_2, NULL);


  while(hardware_init_read_permission_gpio(&alive_led, GPIO_CORE_1_ALIVE_BLINK)<0);
  int8_t gpio_val = gpio_read_state(&alive_led);
  int8_t new_val = gpio_val;

  while (new_val == gpio_val) {
    new_val = gpio_read_state(&alive_led);
  }
  gpio_val = new_val =gpio_read_state(&alive_led);
  PASSED("core 1 alive gpio switched 1");

  while (new_val == gpio_val) {
    new_val = gpio_read_state(&alive_led);
  }
  PASSED("core 1 alive gpio switched 2");

end:
  print_SCORE();
  return err;
}
